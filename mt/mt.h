//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MT_H
#define MACROTEXT_MT_H

#include <variant>
#include <ostream>
#include <vector>
#include <deque>

#include "support/Message.h"
#include "declarations.h"

namespace mt {

    using mtext=std::deque<Token>;
    using parse_result=std::pair<mtext, bool>;
    using iteration=std::pair<size_t,size_t>;
    using plist=std::vector<mtext>;
    struct Instance {
        const plist* parms = nullptr;
        iteration it = {0,0};
        bool generated;     //internal generation via e.g. iForX
        std::string iValue; //substitutes for iForX
        std::string iCount; //substitutes for iForX
        Instance(const plist* p,iteration i,bool gen=false) : parms(p),it(i),generated(gen) {}
        Instance(const Instance&o) : parms(o.parms),it(o.it),generated(o.generated),iValue(o.iValue),iCount(o.iCount) {}
    };
    using Carriage= std::pair<Handler*, Instance>;
    using mstack=std::deque< Carriage >;
}

#include "Wss.h"
#include "Text.h"
#include "Injection.h"
#include "Macro.h"
#include "Content.h"

#include "Driver.h"
#include "Definition.h"


#endif //MACROTEXT_MT_H
