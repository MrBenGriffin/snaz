//
// Created by Ben on 2019-05-29.
//

#include "mt/MacroText.h"
#include "mt/Token.h"
#include "mt/Definition.h"

//#include "mt/using.h"
using namespace std;
using namespace Support;

namespace mt {

	MacroText::MacroText() : tokens() {}

	MacroText::MacroText(MacroText && rhs) noexcept {
		swap(tokens,rhs.tokens);
		rhs.reset();
//		tokens=std::move(rhs.tokens); (swap is MUCH faster than move!)
	}

	void MacroText::adopt(MacroText& rhs) noexcept {
		std::swap(tokens,rhs.tokens);
		rhs.reset();
	}

	void MacroText::reset() {
		tokens.clear();
	}
	
	bool MacroText::simple() const {
		return tokens.empty() ?
		true :
		tokens.size() > 1 ?
		false:
		dynamic_cast<Text *>(tokens.back().get()) != nullptr;
	}

	bool MacroText::empty() const {
		return tokens.empty();
	}


	void MacroText::emplace(unique_ptr<Wss>& token) {
		if (!tokens.empty()) {
			Wss *ptr = dynamic_cast<Wss *>(tokens.back().get());
			if (ptr != nullptr) {
				ptr->text.append(token->text);
			} else {
				tokens.emplace_back(move(token));
			}
		} else {
			tokens.emplace_back(move(token));
		}
	}
	void MacroText::emplace(unique_ptr<Text>& token) {
		if (!tokens.empty()) {
			Text *ptr = dynamic_cast<Text *>(tokens.back().get());
			if (ptr != nullptr) {
				ptr->text.append(token->text);
			} else {
				tokens.emplace_back(move(token));
			}
		} else {
			tokens.emplace_back(move(token));
		}
	}
	void MacroText::emplace(unique_ptr<Macro>& token) {
		tokens.emplace_back(move(token));
	}
	void MacroText::emplace(unique_ptr<Injection>& token) {
		tokens.emplace_back(move(token));
	}
	void MacroText::add(const string& value) {
		if(!value.empty()) {
			if (!tokens.empty()) {
				Text *ptr = dynamic_cast<Text *>(tokens.back().get());
				if (ptr != nullptr) {
					ptr->text.append(value);
				} else {
					auto token = make_unique<Text>(value);
					tokens.emplace_back(move(token));
				}
			} else {
				auto token = make_unique<Text>(value);
				tokens.emplace_back(move(token));
			}
		}
	}
	void MacroText::add(const MacroText& other) {
		for(auto &token : other.tokens) {
			token.get()->clone(*this);
		}
	}
	
	void MacroText::add(const MacroText* other) {
		for(auto &token : other->tokens) {
			token.get()->clone(*this);
		}
	}

	void MacroText::trim() {
		while (!tokens.empty() && dynamic_cast<Wss *>(tokens.front().get()) != nullptr) {
			tokens.pop_front();
		}
		while (!tokens.empty() && dynamic_cast<Wss *>(tokens.back().get()) != nullptr) {
			tokens.pop_back();
		}
	}

	void MacroText::doFor(Messages& log,MacroText& out,const forStuff& stuff,mstack& context) const {
		for (auto& t : tokens) {
			t->doFor(log, out, stuff, context);
		}
	}

//This is a static expansion for quick evaluation.
	std::string MacroText::expand(Messages& errs,std::string& program,mstack& context,bool trim) {
		ostringstream result;
		MacroText _parse;
		MacroText _final;
		istringstream feed(program);
		mt::Driver(errs,feed,mt::Definition::test_adv(program)).parse(errs,_parse,trim);
		_parse.expand(errs,_final,context);
		_final.final(result);
		return result.str();
	}

//Template, Macro, and Parameters all come in here.
//	void expand(Messages&,MacroText&,mstack&) const;
	void MacroText::expand(Messages &e, MacroText &result,mstack &context) const {
		try {
			for(auto& j : tokens) {
				j->expand(e,result,context);
			}
		} catch (exception& ex) {
			e << Message(fatal, ex.what());
		}
	}
	
	void MacroText::expand(Messages&e,std::ostream& o,mstack&context) const {
		MacroText _final;
		try {
			for(auto& j : tokens) {
				j->expand(e,_final,context);
			}
		} catch (exception& ex) {
			e << Message(fatal, ex.what());
		}
		_final.final(o);
	}

	void MacroText::str(Messages &e,std::string& s,mstack &context) const {
		ostringstream o;
		expand(e,o,context);
		s = o.str();
	}

	std::ostream& MacroText::visit(std::ostream& o) const {
		for(auto& j : tokens) {
			j->visit(o);
		}
		return o;
	}

	std::ostream& MacroText::final(std::ostream& o) const {
		for(auto& j : tokens) {
			j->final(o);
		}
		return o;
	}

// Evaluate ONLY injections.
	void MacroText::inject(Messages& e,MacroText& x,mstack& c) const {
		for(auto& i : tokens) {
			i->inject(e,x,c);
		}
	}
// Do substitutes (used by iRegex).
	void MacroText::subs(MacroText& o,const std::deque<std::string>& subs,const std::string& prefix) const {
		for (auto &i : tokens) {
			i->subs(o, subs, prefix);
		}
	}

}
