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
            : counter(0), minParms(min), name(std::move(name_i)), trimParms(trimParms_i), preExpand(preExpand_i) {
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

    void Definition::expand(std::ostream &o,Instance &instance, mstack &context) {
        Instance modified(instance.parms,instance.it);
        plist mt_parms = *instance.parms;
        if (!preExpand) {
            if (trimParms) { trim(mt_parms); }
            std::deque<std::string> rendered;
            for (auto &parm : mt_parms) {
                std::ostringstream in;
                Driver::expand(parm, in, context);
                rendered.push_back(in.str());
            }
            while (!rendered.empty() && rendered.back().empty()) {
                rendered.pop_back();
            }
            mt_parms.clear();
            for (auto &j : rendered) {
                mt_parms.push_back({Text(j)});
            }
            modified.parms = &mt_parms;
            modified.it = {0,mt_parms.size()};
        }
        context.push_front({this, modified});
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

    void Definition::exp(std::string name,std::ostream& o,Instance& i,mt::mstack& c) {
        auto good = Definition::library.find(name);
        if(good != Definition::library.end()) {
            std::visit([&o,&i,&c](auto&& arg){ arg.expand(o,i,c);},good->second);
        }
    }

    void Definition::add(Definition& macro) {
        del(macro.name);
        std::string name(macro.name);
        library.emplace(name,Handler(std::move(macro)));
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
