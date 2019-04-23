//
// Created by Ben on 2019-01-23.
//


#include "support/Convert.h"
#include "mt.h"
namespace mt {
	std::stack<const mtext*> Wss::newline;

	Wss::Wss(const std::string &w) : text(w) {}

    std::string Wss::get() { return text; }
    std::ostream& Wss::visit(std::ostream& o) const {
        o << "‘" << text << "’" << std::flush;
        return o;
    }

	void Wss::push(const mt::mtext *nl) {
		newline.push(nl);
	}

	void Wss::pop() {
		newline.pop();
	}

    void Wss::expand(Messages&,mtext &mt, const mstack &) const {
        if (!mt.empty()) {
			vector<string> notNL;
			Support::tolist(notNL, text, "\n");
			if(notNL.size() == 1) {
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
				for (auto i : notNL) {
					if (std::holds_alternative<Wss>(mt.back())) {
						std::get<Wss>(mt.back()).text.append(i);
					} else {
						if (std::holds_alternative<Text>(mt.back())) {
							std::get<Text>(mt.back()).append(i);
						} else {
							mt.emplace_back(Text(i));
						}
					}
					if (i != notNL.back() && !newline.empty() && newline.top() != nullptr) {
						for( auto x : *newline.top()) {
							mt.emplace_back(x);
						}
					}
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

