//
// Created by Ben on 2019-01-24.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H


#include "Definition.h"
#include "mt.h"

namespace mt {
    struct iEq { void expand(std::ostream &,Instance&,mstack&); };
}


#endif //MACROTEXT_INTERNALS_H
