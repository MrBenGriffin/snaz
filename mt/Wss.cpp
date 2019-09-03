#include <utility>

//
// Created by Ben on 2019-01-23.
//

#include "support/Convert.h"
#include "mt.h"
namespace mt {
	MacroText Wss::empty = MacroText();

	std::stack<const MacroText*> Wss::newline;
	Wss::Wss(std::string w) : Script(std::move(w)) {
	}
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
//	There are a few characters which can indicate a new line.
//	The usual ones are these two: '\n' or '0x0A' (10 in decimal) ->
//	This character is called "Line Feed" (LF). '\r' or '0x0D' (13 in decimal) ->
//	This one is called "Carriage return" (CR).
	void Wss::expand(Messages&e ,MacroText &out, mstack &context) const {
		if (!text.empty()) {
			vector<string> notNL;
//			Support::fandr(basis, "␍", "\x0D");
//			Support::fandr(basis, "␊", "\x0A");
			if (string::npos == text.find("\x0D\x0A")) {
				Support::tolist(notNL, text, "\x0D\x0A" );
			} else {
				if (string::npos == text.find("\x0D")) {
					Support::tolist(notNL, text, "\x0D"  );
				} else {
					if (string::npos == text.find("\x0A")) {
						Support::tolist(notNL, text, "\x0A"  );
					} else {
						// This is a tab or tabs..
						// Not sure I know why this is here..
						auto token=make_unique<Text>(this->text);
						out.emplace(token);
					}
				}
			}
			if(!notNL.empty()) {
				for (auto& i : notNL) {
					if(!i.empty()) {
						out.add(i);
					}
					if (i != notNL.back() && !newline.empty()) {
						auto* current = newline.top();
						if(!current->empty()) {
							newline.push(&empty);  // Otherwise newline top may try to expand itself.
							current->expand(e,out,context);
							newline.pop();
						}
					}
				}
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
