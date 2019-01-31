//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
namespace mt {

    Text::Text(const std::string &w) : text(w) {}

    std::ostream &Text::visit(std::ostream &o) const {
        o << "“" << text << "”" << std::flush;
        return o;
    }

    std::string Text::get() const { return text; }
    void Text::append(std::string right) { text.append(right); };

    void Text::expand(Messages&,mtext &mt, const mstack &) const {
        if (!mt.empty()) {
            if (std::holds_alternative<Text>(mt.back())) {
                std::get<Text>(mt.back()).text.append(text);
            } else {
                if (std::holds_alternative<Wss>(mt.back())) {
                    std::string ws = std::get<Wss>(mt.back()).get();
                    mt.pop_back();
                    ws.append(text);
                    mt.emplace_back(std::move(Text(ws)));
                } else {
                    mt.emplace_back(*this);
                }
            }
        } else {
            mt.emplace_back(*this);
        }
    }

    void Text::add(mtext &mt) {
        if (!mt.empty() && std::holds_alternative<Text>(mt.back())) {
            text = std::get<Text>(mt.back()).text + text;
            mt.pop_back();
        }
        mt.emplace_back(std::move(*this));
    }

}