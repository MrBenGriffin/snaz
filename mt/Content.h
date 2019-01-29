//
// Created by Ben on 2019-01-29.
//

#ifndef MACROTEXT_CONTENT_H
#define MACROTEXT_CONTENT_H

#include <string>
#include "mt.h"

namespace mt {

    struct Content {
        std::string _name;
        std::string name() const { return _name; }
        bool inRange(size_t i) const { return  i == 0;}
        Content(std::string name) : _name(name) {}
        std::ostream &visit(std::ostream &o ) { return o; }
        void expand(mtext&,Instance&,mstack&) {}

    };

}

#endif //MACROTEXT_CONTENT_H
