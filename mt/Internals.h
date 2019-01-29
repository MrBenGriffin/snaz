//
// Created by Ben on 2019-01-24.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H


#include "Definition.h"
#include "mt.h"

namespace mt {
    struct Internal {
        std::string _name;
        long minParms {0}, maxParms {0};
        bool inRange(size_t i) const { return minParms <= i <= maxParms;}
        std::string name() const {return _name;}
        Internal(std::string name,size_t min,size_t max) : _name(name),minParms(min),maxParms(max) {}
    };
    struct iEq : public Internal {
        iEq() : Internal("iEq",0,4) {}
        void expand(mtext&,Instance&,mstack&);
    };

}


#endif //MACROTEXT_INTERNALS_H
