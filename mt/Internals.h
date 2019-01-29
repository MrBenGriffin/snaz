//
// Created by Ben on 2019-01-24.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H


#include "Definition.h"
#include "mt.h"

namespace mt {
    struct Internal {
        size_t minParms {0}, maxParms {0};
        Internal(size_t min,size_t max) : minParms(min),maxParms(max) {}
    };
    struct iEq : public Internal {
        iEq() : Internal(0,4) {}
        void expand(mtext&,Instance&,mstack&);
    };

}


#endif //MACROTEXT_INTERNALS_H
