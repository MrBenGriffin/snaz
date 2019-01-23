//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
namespace mt {
    Wss::Wss(const std::string &w) : text(w) {}

    std::ostream& Wss::visit(std::ostream& o) {
        o << "‘" << text << "’" << std::flush;
        return o;
    }
    void Wss::expand(std::ostream& o,const mstack&,const iteration) {
        o << text;
    }

    void Wss::add(mtext& mt) {
        if (!mt.empty() && std::holds_alternative<Wss>(mt.back())) {
            text = std::get<Wss>(mt.back()).text + text;
            mt.pop_back();
        }
        mt.emplace_back(std::move(*this));
    }

}

