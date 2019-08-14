//
// Created by Ben on 2019-01-10.
//

#ifndef SNAZZLE_TEST_H

#include <tuple>
#include <string>
#include <vector>

#include "support/Message.h"
using namespace std;
using namespace Support;

namespace testing {

    using test = tuple<string,string,string>;
	constexpr const char* df = "+";
	constexpr const char* dm = "-";
	constexpr const char* dn = "+";
	constexpr const size_t lineWidth = 96;

    class group {
        string base;
/*
        Black            \e[0;30m
 √      Blue             \e[0;34m
 √      Green            \e[0;32m
        Cyan             \e[0;36m
 √      Red              \e[0;31m
        Purple           \e[0;35m
        Brown            \e[0;33m
        Gray             \e[0;37m
        Dark Gray        \e[1;30m
        Light Blue       \e[1;34m
        Light Green      \e[1;32m
        Light Cyan       \e[1;36m
        Light Red        \e[1;31m
        Light Purple     \e[1;35m
        Yellow           \e[1;33m
        White            \e[1;37m
 */
        const std::string red    = "\033[1;31m";
        const std::string lred   = "\033[0;31m";
        const std::string green  = "\033[1;32m";
        const std::string blue   = "\033[1;34m";
        const std::string purple = "\033[1;35m";
        const std::string norm   = "\033[0;37m";
        void wss(std::string&,bool);
    protected:
        vector<test> tests;

    public:
        Messages* msgs;
        group(Messages&,string);
        void title(ostream&,string,int = 0);
        void load(ostream&,string,bool = false,bool = false);
    };

}

#endif //SNAZZLE_TEST_H
