//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
#include "userMacro.h"

namespace mt {

    Macro::Macro(std::string n) : name(std::move(n)) {};

    void Macro::expand(std::ostream &o,mstack &context,const iteration i) {
        if (userMacro::has(name)) {
            userMacro::get(name).expand(o, parms, context);
        } else {
            visit(o);
        }
    }

    std::ostream &Macro::visit(std::ostream &result) {
        result << "⸠" << name;
        for (auto &p : parms) {
            result << "「";
            if (!p.empty()) {
                Driver::visit(p, result);
            }
            result << "」";
        }
        return result;
    }

    void Macro::add(mtext& mt) {
        mt.emplace_back(std::move(*this));
    }

}

