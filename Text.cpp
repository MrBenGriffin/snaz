//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
namespace mt {

    Text::Text(const std::string &w) : text(w) {}

    std::ostream& Text::visit(std::ostream& o) {
        o << "“" << text << "”" << std::flush;
        return o;
    }
    void Text::expand(std::ostream& o,const mstack&,const iteration) {
        o << text;
    }

    void Text::add(mtext& mt) {
        if (!mt.empty() && std::holds_alternative<Text>(mt.back())) {
            text = std::get<Text>(mt.back()).text + text;
            mt.pop_back();
        }
        mt.emplace_back(std::move(*this));
    }

}