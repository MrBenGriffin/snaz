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
//	There are a few characters which can indicate a new line. The usual ones are these two: '\n' or '0x0A' (10 in decimal) -> This character is called "Line Feed" (LF). '\r' or '0x0D' (13 in decimal) -> This one is called "Carriage return" (CR).24 Jul 2005
	void Wss::expand(Messages&,MacroText &out, mstack &) const {
		if (!out.empty()) {
			vector<string> notNL;
//			Support::fandr(basis, "␍", "\x0D");
//			Support::fandr(basis, "␊", "\x0A");

			Support::tolist(notNL, text, "\x0D\x0A"  );
			if(notNL.size() == 1) {
				notNL.clear();
				Support::tolist(notNL, text, "\x0A");
			}
			if (notNL.size() == 1) {
				notNL.clear();
				Support::tolist(notNL, text, "\x0D");
			}
			if(notNL.size() == 1) {
				if(!text.empty()) {
					auto token=make_unique<Wss>(this->text);
					out.emplace(token);
				}
			} else {
				for (auto& i : notNL) {
					if(!i.empty()) {
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
