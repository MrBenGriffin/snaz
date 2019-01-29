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

    //The following are Token classes.
    class Macro;
    class Injection;
    class Wss;
    class Text;

    class iEq; class Definition; class Content;
    using Handler=std::variant<Content,Definition,iEq>;

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
