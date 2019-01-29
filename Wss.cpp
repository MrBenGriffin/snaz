//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
namespace mt {
    Wss::Wss(const std::string &w) : text(w) {}

    std::string Wss::get() { return text; }
    std::ostream& Wss::visit(std::ostream& o) const {
        o << "‘" << text << "’" << std::flush;
        return o;
    }

    void Wss::expand(mtext &mt, const mstack &) const {
        if (!mt.empty()) {
            if (std::holds_alternative<Wss>(mt.back())) {
                std::get<Wss>(mt.back()).text.append(text);
            } else {
                if (std::holds_alternative<Text>(mt.back())) {
                    std::get<Text>(mt.back()).append(text);
                } else {
                    mt.emplace_back(*this);
                }
            }
        } else {
            mt.emplace_back(*this);
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

