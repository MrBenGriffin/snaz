//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_TEXT_H
#define MACROTEXT_TEXT_H

#include <string>
#include "mt.h"

namespace mt {
    class Text {
    private:
        std::string text;
    public:
        Text() = default;
        Text(const std::string &);
        std::ostream& visit(std::ostream&);
        void expand(std::ostream&,const mstack&,const iteration);
        void add(mtext&);
        bool empty() const { return text.empty(); }
    };

};


#endif //MACROTEXT_TEXT_H
