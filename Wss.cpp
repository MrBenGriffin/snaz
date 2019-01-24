//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
namespace mt {
    Wss::Wss(const std::string &w) : text(w) {}

    std::ostream& Wss::visit(std::ostream& o) const {
        o << "‘" << text << "’" << std::flush;
        return o;
    }

    void Wss::expand(mtext& o,const mstack&) const {
        if (!o.empty() && std::holds_alternative<Wss>(o.back())) {
            std::get<Wss>(o.back()).text.append(text);
        } else {
            o.emplace_back(*this);
        }
     }

    void Wss::add(mtext& mt) {
        if (!mt.empty() && std::holds_alternative<Wss>(mt.back())) {
            text = std::get<Wss>(mt.back()).text + text;
            mt.pop_back();
        }
        mt.emplace_back(std::move(*this));
    }

}

