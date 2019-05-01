//
// Created by Ben on 2019-01-29.
//

#ifndef MACROTEXT_CONTENT_H
#define MACROTEXT_CONTENT_H

#include <string>
#include "mt/mt.h"
#include "mt/Handler.h"

namespace mt {
    using namespace Support;
    class Content : public Handler {
        std::string _name;
        std::string name() const;
        bool inRange(size_t i) const;
        Content(std::string name);
      };
}

#endif //MACROTEXT_CONTENT_H
