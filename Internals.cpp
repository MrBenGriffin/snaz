//
// Created by Ben on 2019-01-24.
//

#include "Internals.h"

namespace mt {
    void iEq::expand(std::ostream& o,Instance& instance,mstack& context) {
//        auto &contextMacro = context[sValue].first;
//        auto &instance = context[sValue].second;
        auto &myParms = instance.parms;
//        auto &iter = instance.it;

        auto size = myParms->size();
        std::string a,b;
        if (size > 0) {
            std::ostringstream left;
            Driver::expand((*myParms)[0], left, context);
            if (size > 1) {
                std::ostringstream right;
                Driver::expand((*myParms)[1], right, context);
                a = left.str(); b=right.str();
                if (size > 2) {
                    if (left.str() == right.str()) {
                        Driver::expand((*myParms)[2], o, context);
                    } else {
                        if (size > 3) {
                            Driver::expand((*myParms)[3], o, context);
                        } //else return empty-string.
                    }
                } else {
                    o << (left.str() == right.str() ? "1" : "0");
                }
            } else {
                o << (left.str().empty() ? "1" : "0");
            }
        }
    }
}