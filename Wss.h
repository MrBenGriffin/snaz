//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_WSS_H
#define MACROTEXT_WSS_H

#include <string>
#include "mt.h"


namespace mt {

    class Wss {
    private:
        std::string text;
    public:
        Wss(const std::string &);
        std::ostream& visit(std::ostream&);
        void expand(std::ostream&,const mstack&,const iteration);
        void add(mtext&);
        bool empty() const { return text.empty(); }

    };
}

#endif //MACROTEXT_WSS_H
