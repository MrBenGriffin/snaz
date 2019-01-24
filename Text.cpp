//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
namespace mt {

    Text::Text(const std::string &w) : text(w) {}

    std::ostream& Text::visit(std::ostream& o) const {
        o << "“" << text << "”" << std::flush;
        return o;
    }

    std::string Text::get() { return text; }
    void Text::expand(mtext& o,const mstack&) const {
        if (!o.empty() && std::holds_alternative<Text>(o.back())) {
            std::get<Text>(o.back()).text.append(text);
        } else {
            o.emplace_back(*this);
        }
    }

    void Text::add(mtext& mt) {
        if (!mt.empty() && std::holds_alternative<Text>(mt.back())) {
            text = std::get<Text>(mt.back()).text + text;
            mt.pop_back();
        }
        mt.emplace_back(std::move(*this));
    }

}