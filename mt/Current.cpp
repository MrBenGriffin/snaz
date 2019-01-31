//
// Created by Ben on 2019-01-29.
//

#include "Current.h"
#include "Internals.h"

namespace mt {

    Current::Current(const Internal *thing,Messages& e,mtext &o, Instance &i, mstack &c) :
        output(&o), instance(&i), context(&c),errs(&e) {
        parms = i.parms;
        count = parms->size();
        min = thing->minParms;
        max = thing->maxParms;
    }

    std::string Current::parm(size_t i) {
        std::ostringstream result;
        Driver::expand((*parms)[i - 1],*errs, result, *context);
        return result.str();
    }

    void Current::expand(size_t i) {
        Driver::expand((*parms)[i - 1],*errs, *output, *context);
    }

    void Current::set(std::string str) {
        output->emplace_back(Text(str));
    }

    void Current::logic(size_t offset) { //offset is 1-indexed! This is a string comparison starting with the parm at offset.
        if (count >= offset) {
            std::string left = parm(offset);
            std::string right = count > offset ? parm(offset+1): "";

            bool value = left == right;
            if(count > offset+1) {
                if (value) {
                    expand(offset + 2);
                } else {
                    if (count > offset + 2) {
                        expand(offset + 3);
                    } //else return empty-string.
                }
            } else {
                set(value ? "1" : "0");
            }
        }
    }
}