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
		tokens=std::move(rhs.tokens);
	}

	void MacroText::adopt(MacroText& rhs) noexcept {
		std::swap(tokens,rhs.tokens);
		rhs.reset();
	}

	void MacroText::reset() {
		tokens.clear();
	}
	inline bool MacroText::simple() const {
		return tokens.empty() ?
		true :
		tokens.size() > 1 ?
		false:
		dynamic_cast<Text *>(tokens.back().get()) != nullptr;
	}

	inline bool MacroText::empty() const {
		return tokens.empty();
	}
	
//	void emplace(unique_ptr<Wss>&);
//	void emplace(unique_ptr<Text>&);
//	void emplace(unique_ptr<Macro>&);
//	void emplace(unique_ptr<Injection>&);

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
		if (!tokens.empty()) {
			Text *ptr = dynamic_cast<Text *>(tokens.back().get());
			if (ptr != nullptr) {
				ptr->text.append(value);
			} else {
				auto token=make_unique<Text>(value);
				tokens.emplace_back(move(token));
			}
		} else {
			auto token=make_unique<Text>(value);
			tokens.emplace_back(move(token));
		}
	}
	void MacroText::add(const MacroText& other) {
		for(auto &token : other.tokens) {
			tokens.emplace_back(token.get()->clone());
		}
	}
	
	void MacroText::add(const MacroText* other) {
		for(auto &token : other->tokens) {
			tokens.emplace_back(token.get()->clone());
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

	void MacroText::doFor(MacroText& out,const forStuff& stuff) const {
		for (auto& t : tokens) {
			t->doFor(out, stuff);
		}
	}

// Migrated from Driver!
//	void doFor(MacroText&,const forStuff&) const;
//	void expand(Messages&,std::ostream&,mstack&) const;
//	void inject(Messages&,MacroText&,mstack&) const;
//	void subs(MacroText&,const std::vector<std::string>&,const std::string&) const;
//	std::ostream& visit(std::ostream&) const;
//
//	static std::string expand(Messages&,std::string&,mstack&); //dirty
//	static std::ostream& visit(const Token&, std::ostream&);

//This is a static expansion for quick evaluation.
	std::string MacroText::expand(Messages& errs,std::string& program,mstack& context) {
		ostringstream result;
		MacroText _parse;
		MacroText _final;
		istringstream feed(program);
		mt::Driver(errs,feed,mt::Definition::test_adv(program)).parse(errs,_parse,false);
		_parse.expand(errs,_final,context);
		_final.visit(result);
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

	std::ostream& MacroText::visit(std::ostream& o) const {
		for(auto& j : tokens) {
			j->visit(o);
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
	void MacroText::subs(const std::vector<std::string>& subs,const std::string& prefix) {
		for (auto &i : tokens) {
			i->subs(subs, prefix);
		}
	}

}
