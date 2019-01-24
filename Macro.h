//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MACRO_H
#define MACROTEXT_MACRO_H

#include <string>
#include "mt.h"


namespace mt {
    /**
    * an instance of this is a macro token instance in a piece of text.
    * It may be PART of a usermacro definition.
    * With a context, it can be evaluete
   **/
    class Macro {
    public:
        std::string name;
        plist parms;        /* parsed parms */
        void expand(mtext&,mstack&) const;
        std::ostream& visit(std::ostream&) const;
        explicit Macro(std::string);
        void add(mtext&);
    };
}

#endif //MACROTEXT_MACRO_H
