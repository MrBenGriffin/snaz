#include <utility>

//
// Created by Ben on 2019-01-23.
//

#include "support/Convert.h"
#include "mt.h"
namespace mt {
	std::stack<const MacroText*> Wss::newline;
	Wss::Wss(std::string w) : Script(std::move(w)) {}
	Wss::Wss(const Wss* o) : Script(o->text) {
		if(o->text.empty()) {
			throw bad_alloc();
		}
	}
	std::ostream& Wss::visit(std::ostream& o) const {
		o << "‘" << text << "’" << std::flush;
		return o;
	}
	void Wss::push(const MacroText* nl) {
		newline.push(nl);
	}
	void Wss::pop() {
		newline.pop();
	}
	
	void Wss::clone(MacroText &out) const {
		if(!text.empty()) {
			auto token=make_unique<Wss>(this->text);
			out.emplace(token);
		}
	}

	void Wss::expand(Messages&,MacroText &out, mstack &) const {
		if (!out.empty()) {
			vector<string> notNL;
			Support::tolist(notNL, text, "\n");
			if(notNL.size() == 1) {
				if(!text.empty()) {
					auto token=make_unique<Wss>(this->text);
					out.emplace(token);
				}
			} else {
				for (auto& i : notNL) {
					if(!text.empty()) {
						out.add(i);
					}
					if (i != notNL.back() && !newline.empty()) {
						out.add(newline.top());
					}
				}
			}
		} else {
			if(!text.empty()) {
				auto token=make_unique<Wss>(this->text);
				out.emplace(token);
			}
		}
	}

	void Wss::inject(Messages&,MacroText& out,mstack&) const {
		if(!text.empty()) {
			auto token=make_unique<Wss>(this->text);
			out.emplace(token);
		}
	}

	void Wss::subs(MacroText& out,const std::vector<std::string>&,const std::string&) const {
		auto token=make_unique<Wss>(this->text);
		out.emplace(token);
	}

}
