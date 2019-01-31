//
// Created by Ben on 2019-01-24.
//
#include <iostream>

#include "Internals.h"
#include "Current.h"
#include "support/Infix.h"
#include "support/Message.h"

namespace mt {
    using namespace Support;
    void iEq::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
        Current my(this,e,o,instance,context);
        my.logic(1);
    }

    void iExpr::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
        Current my(this,e,o,instance,context);
        Infix::Evaluate expression;
        auto expr=my.parm(1);
        expression.set_expression(expr);
        for (size_t i=2; i <= my.count; i++) {
            char **end = nullptr;
            std::string parmValue = my.parm(i);
            std::ostringstream ost;
            ost << "p" << i-1; //parm by number.
            auto number = strtold(&parmValue[0], end);
            expression.add_parm(ost.str(),number);
        }
        long double value = expression.process(*my.errs);
        if(my.errs->marked()) {
            my.errs->prefix({usage,expr});
            my.errs->enscope("Within iExpr");
        }
        std::ostringstream ost; ost << value;
        my.set(ost.str());
    }

}
