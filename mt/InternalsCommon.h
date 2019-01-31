//
// Created by Ben on 2019-01-29.
//

#ifndef MACROTEXT_CURRENT_H
#define MACROTEXT_CURRENT_H


#include "Definition.h"
#include "mt.h"

namespace mt {

    using namespace Support;
    // Used by Internals to hold current evaluation,
    // and also manages common utility functions.
    class InternalsCommon {
     public:
        size_t          min {0}, max{0}, count {0};
        mtext*          output {nullptr};
        Instance*       instance {nullptr};
        const plist*    parms {nullptr};
        mstack*         context {nullptr};
        Messages*       errs;
        InternalsCommon(const Internal*,Support::Messages&,mtext&,Instance&,mstack&);

        std::string     parm(size_t);
        void            expand(size_t);
        void            set(std::string);
        void            logic(bool,size_t);
        void            logic(std::string&,size_t);
    };

}

#endif //MACROTEXT_CURRENT_H
