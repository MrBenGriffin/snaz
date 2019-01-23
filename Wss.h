//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_WSS_H
#define MACROTEXT_WSS_H

#include <string>


namespace mt {
    class Wss {
    public:
        std::string text;
        Wss(const std::string &);
        Wss(Wss&,const std::string &);

    };
}

#endif //MACROTEXT_WSS_H
