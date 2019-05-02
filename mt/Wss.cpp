#include <utility>

//
// Created by Ben on 2019-01-23.
//


#include "support/Convert.h"
#include "mt.h"
namespace mt {
	std::stack<const mtext*> Wss::newline;

	Wss::Wss(std::string w) : text(std::move(w)) {}

	bool Wss::empty() const { return text.empty(); }

    std::string Wss::get() const { return text; }

	void Wss::final(std::ostream& o) const {
		o << text;
	}

	std::ostream& Wss::visit(std::ostream& o) const {
        o << "‘" << text << "’" << std::flush;
        return o;
    }

	void Wss::push(const mtext* nl) {
		newline.push(nl);
	}

	void Wss::pop() {
		newline.pop();
	}

    void Wss::expand(Messages&,mtext &mt, mstack &) const {
        if (!mt.empty()) {
			Token *back = mt.back().get();
			vector<string> notNL;
			Support::tolist(notNL, text, "\n");
			if(notNL.size() == 1) {
				Wss *wssPtr = dynamic_cast<Wss *>(back);
				if (wssPtr != nullptr) {
					wssPtr->text.append(text);
				} else {
					Text *textPtr = dynamic_cast<Text *>(back);
					if (textPtr != nullptr) {
						textPtr->append(text);
					} else {
						mt.emplace_back(const_cast<Wss*>(this)); //because this method is const, this holds const.
					}
				}
			} else {
				for (auto& i : notNL) {
					back = mt.back().get();
					auto* wssPtr = dynamic_cast<Wss *>(back);
					if (wssPtr != nullptr) {
						wssPtr->text.append(i);
					} else {
						auto* textPtr = dynamic_cast<Text *>(back);
						if (textPtr != nullptr) {
							textPtr->append(i);
						} else {
							mt.emplace_back(new Text(i));
						}
					}
					if (i != notNL.back() && !newline.empty() && (newline.top() != nullptr)) {
						auto* nl = newline.top();
						for( auto& x : *nl) { //each token..;
							mt.push_back(x);
						}
					}
				}
			}
        } else {
            mt.emplace_back(const_cast<Wss*>(this));
        }
    }

    void Wss::add(mtext& mt) {
		if (!mt.empty()) {
			auto back = mt.back();
			Wss *ptr = dynamic_cast<Wss *>(back.get());
			if (ptr != nullptr) {
				text = ptr->text + text;
				mt.pop_back();
			}
		}
		mt.emplace_back(this);
    }

}

