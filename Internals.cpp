//
// Created by Ben on 2019-01-24.
//

#include "Internals.h"
#include "Current.h"

namespace mt {
    void iEq::expand(mtext& o,Instance& instance,mstack& context) {
        Current my(this,o,instance,context);
        my.logic(1);
    }
}
