//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MT_H
#define MACROTEXT_MT_H

#include <variant>
#include <ostream>
#include <vector>
#include <deque>


namespace mt {

    class Macro;
    class Injection;
    class Wss;
    class Text;
    class Definition;

    using Token=std::variant<Macro,Wss,Injection,Text>;
    using mtext=std::deque<Token>;
    using parse_result=std::pair<mtext, bool>;
    using iteration=std::pair<size_t,size_t>;
    using plist=std::vector<mtext>;
    struct Instance {
        const plist* parms = nullptr;
        iteration it = {0,0};
        Instance(const plist* p,iteration i) : parms(p),it(i) {}
    };

    using mstack=std::deque<std::pair<Definition*, Instance> >;
}

#include "Wss.h"
#include "Text.h"
#include "Injection.h"
#include "Macro.h"
#include "Driver.h"
#include "Definition.h"

#endif //MACROTEXT_MT_H
