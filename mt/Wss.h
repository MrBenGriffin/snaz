//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_WSS_H
#define MACROTEXT_WSS_H

#include<stack>
#include <string>
#include "mt.h"


namespace mt {
    using namespace Support;
    class Wss {
    private:
    	static std::stack<const mtext*> newline;
        std::string text;
    public:
        Wss(const std::string &);
        std::string get();
        std::ostream& visit(std::ostream&) const;
        void expand(Messages&,mtext&,const mstack&) const;
        void add(mtext&);
        bool empty() const { return text.empty(); }
        static void push(const mtext*);
		static void pop();

    };
}

#endif //MACROTEXT_WSS_H
