//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MT_H
#define MACROTEXT_MT_H

#include <variant>
#include <ostream>
#include <vector>
#include <deque>

class userMacro;

namespace mt {

    class Macro;
    class Injection;
    class Wss;
    class Text;

    using Token=std::variant<Macro,Wss,Injection,Text>;
    using mtext=std::deque<Token>;
    using parse_result=std::pair<mtext, bool>;
    using plist=std::vector<mtext>;
    using mstack=std::deque<std::pair<userMacro*, plist>>;
    using iteration=std::pair<size_t,size_t>;
}

#include "Wss.h"
#include "Text.h"
#include "Injection.h"
#include "Macro.h"
#include "Driver.h"

#endif //MACROTEXT_MT_H
