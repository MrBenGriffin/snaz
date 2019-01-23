//
// Created by Ben on 2019-01-23.
//

#include "Wss.h"

namespace mt {
    Wss::Wss(const std::string &w) : text(w) {}

    Wss::Wss(Wss &o, const std::string &w) {
        text = std::move(o.text);
        text.append(w);
    };
}

