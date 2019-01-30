#include <utility>

//
// Created by Ben on 2019-01-10.
//

#include <utility>
#include <variant>
#include "mt.h"
#include "Internals.h"

namespace mt {

    std::unordered_map<std::string, Handler> Definition::library;
    Definition Definition::empty("__empty", "", false, false, false);

    Definition::Definition(
            std::string name_i, std::string expansion_i,
            long min, long max, bool strip, bool trimParms_i, bool preExpand_i)
            : counter(0), minParms(min), _name(std::move(name_i)), trimParms(trimParms_i), preExpand(preExpand_i) {
        maxParms = max == -1 ? INT_MAX : max;
        if(expansion_i.empty()) {
            expansion = {};
            iterated = false;
        } else {
            Driver driver;
            std::istringstream code(expansion_i);
            bool advanced = test_adv(expansion_i);
            parse_result result = driver.define(code, advanced, strip);
            expansion = result.first;
            iterated = result.second;
        }
    }

    void Definition::expand(mtext& o,Instance &instance, mstack &context) {
        Instance modified(nullptr,instance.it);
        plist rendered;
        if (!preExpand) {
            plist mt_parms = *instance.parms;
            if (trimParms) { trim(mt_parms); }
             modified.parms = &rendered;
             context.push_back({nullptr,modified}); //This is done for injections like %(1+).
             for (auto &parm : mt_parms) {
                 mtext expanded;
                 auto i=rendered.size();
                 Driver::expand(parm, expanded, context);
                 if(rendered.size() == i) {
                     rendered.push_back(std::move(expanded)); //each one as a separate parm!!
                 }
             }
            context.pop_back();
            while (!rendered.empty() && rendered.back().empty()) {
                rendered.pop_back();
            }
            modified.it = {0,rendered.size()};
        }
        Handler handler(*this);
        context.push_front({&handler,modified});
        if (iterated) {
            iteration* i = &(context.front().second.it);
             for (i->first = 1; i->first <= i->second; i->first++) {
                Driver::expand(expansion, o, context);
            }
        } else {
            Driver::expand(expansion, o, context);
        }
        context.pop_front();
    }


    bool Definition::test_adv(std::string &basis) {
        //⌽E2.8C.BD ⍟E2.8D.9F ⎣E2.8E.A3
        if (!basis.empty()) {
            size_t size = basis.size();
            size_t offset = basis.find(0xE2);
            for (; offset != std::string::npos && offset + 2 < size; offset += 2) {
                char a = basis[offset + 1];
                char b = basis[offset + 2];
                if (a == char(0x8C) && b == char(0xBD)) return true;
                if (a == char(0x8D) && b == char(0x9F)) return true;
                if (a == char(0x8E) && b == char(0xA3)) return true;
                offset = basis.find(0xE2, offset);
            }
        }
        return false;
    }

    void Definition::vis(std::string name,std::ostream& o) {
        auto good = Definition::library.find(name);
        if(good != Definition::library.end()) {
            if (std::holds_alternative<Definition>(good->second)) {
                std::get<Definition>(good->second).visit(o);
            } else {
                o << "«" << name << "»";
            }
         }
    }

    void Definition::exp(std::string name,mtext& result,Instance& i,mt::mstack& c) {
        auto good = Definition::library.find(name);
        if(good != Definition::library.end()) {
            std::visit([&result,&i,&c](auto&& arg){ arg.expand(result,i,c);},good->second);
        }
    }

    void Definition::add(Definition& macro) {
        del(macro.name());
        std::string name(macro.name());
        library.emplace(macro.name(),Handler(std::move(macro)));
    }

    bool Definition::has(std::string name) {
        return library.find(name) != Definition::library.end();
    }

    void Definition::del(std::string name) {
        auto good = library.find(name);
        if (good != library.end()) {
            library.erase(good);
        }
    }

    void Definition::list(std::ostream& o) {
        for (auto& a :library) {
            o << a.first << ": ";
            vis(a.first,o);
            o << std::endl;
        }
    }


    std::ostream &Definition::visit(std::ostream &o) {
        return Driver::visit(expansion, o);
    }

    void Definition::trim(plist &bobs) {
        for (auto &j : bobs) { //each parameter.
            while (!j.empty() && std::holds_alternative<Wss>(j.front())) {
                j.pop_front();
            }
            while (!j.empty() && std::holds_alternative<Wss>(j.back())) {
                j.pop_back();
            }
        }
    }

    void Definition::initialise() {
        library.emplace("iEq",Handler(std::move(iEq())));
    }


}
