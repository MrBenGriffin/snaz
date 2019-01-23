//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_INJECTION_H
#define MACROTEXT_INJECTION_H

#include <string>
#include "mt.h"

namespace mt {

    class Injection {

    private:
        enum class It { plain,text,current,count,size }; //not iteration,not injection,i,j,k,n respectively
        It type;            // tells us the iteration type
        size_t value;       // the parameter number being asked for (when plain)
        size_t sValue;      // the stack offset
        long offset;        // the offset value (signed!)
        bool modulus;       // if the offset is a modulus.
        bool stack;         // if this is a full stack trace
        bool list;          // if we are passing a list of parameters over (eg %(3+))
        std::string basis;  // What we are working with.

        void parseStart();
        void parseIterated();
        void parsePName();
        void parseBrackets();
        void parseParent();

    public:
        Injection();
        Injection(const std::string);
        Injection(const Injection& ) = default;
        bool iterator;      // This is injection defines the macro as an iterator.
        std::ostream& visit(std::ostream&);
        void expand(std::ostream&,mstack&,const iteration);

    };
}

#endif //MACROTEXT_INJECTION_H
