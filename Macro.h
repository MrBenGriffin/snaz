//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MACRO_H
#define MACROTEXT_MACRO_H

#include <string>
#include "mt.h"


namespace mt {
    /**
    * This is the parser-macro.
    * It forms a part of the macrotext.
    * It needs context to be an instance.
    * It may be PART of a usermacro definition.
   **/
    class Macro {
    public:
        std::string name;
        plist parms;
        void expand(std::ostream&,mstack&);
        std::ostream& visit(std::ostream&);
        explicit Macro(std::string);
    };
}


#endif //MACROTEXT_MACRO_H
