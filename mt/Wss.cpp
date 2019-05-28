#include <utility>

//
// Created by Ben on 2019-01-23.
//


#include "support/Convert.h"
#include "mt.h"
namespace mt {
	std::stack<const mtext*> Wss::newline;

	Wss::Wss(const std::string& w) : Script(w) {}
	Wss::Wss(const std::ostringstream& s) : Script(s) {}

	std::ostream &Wss::visit(std::ostream &o) const {
		o << "‘" << text.str() << "‘" << std::flush;
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
			Support::tolist(notNL, text.str(), "\n");
			if(notNL.size() == 1) {
				Wss *wssPtr = dynamic_cast<Wss *>(back);
				if (wssPtr != nullptr) {
					wssPtr->append(text);
				} else {
					auto *textPtr = dynamic_cast<Script *>(back);
					if (textPtr != nullptr) {
						textPtr->append(text);
					} else {
						shared_ptr<Token> ptr = make_shared<Wss>(text);
						mt.emplace_back(ptr);
					}
				}
			} else {
				for (auto& i : notNL) {
					back = mt.back().get();
					auto* wssPtr = dynamic_cast<Wss *>(back);
					if (wssPtr != nullptr) {
						wssPtr->append(i);
					} else {
						auto* textPtr = dynamic_cast<Script *>(back);
						if (textPtr != nullptr) {
							textPtr->append(i);
						} else {
							shared_ptr<Token> ptr = make_shared<Wss>(text);
							mt.emplace_back(ptr);
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
			shared_ptr<Token> ptr = make_shared<Wss>(text);
			mt.emplace_back(ptr);
		}
	}

	void Wss::subs(mtext& out,const std::vector<std::string>&,const std::string&) const {
		shared_ptr<Token> ptr = make_shared<Wss>(text);
		out.emplace_back(ptr);
	}


}

