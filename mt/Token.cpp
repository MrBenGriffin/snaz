//
// Created by Ben on 2019-05-01.
//

#include "mt/declarations.h"
#include "mt/Token.h"
#include "mt/Macro.h"
#include "mt/Wss.h"
#include "mt/Text.h"
#include "mt/Injection.h"

namespace mt {

	void Token::expand(Messages&,mtext&,mstack&) const {}
	void Token::doFor(mtext&,const forStuff&) const {}
	bool Token::empty() const { return true; }
	void Token::subs(mtext& out,const std::vector<std::string>&,const std::string&) const {
		out.emplace_back(const_cast<Token*>(this));
	}
	void Token::inject(Messages&,mtext& out,mstack&) const {
		out.emplace_back(const_cast<Token*>(this));
	}

	void Token::add(Token* token,mtext& mt) {
		Wss  *w = dynamic_cast<Wss *>(token);
		if(w != nullptr) {
			if (!mt.empty()) {
				auto back = mt.back();
				Wss *ptr = dynamic_cast<Wss *>(back.get());
				if (ptr != nullptr) {
					w->text = ptr->text + w->text;
					mt.pop_back();
				}
			}
		} else {
			Text *t = dynamic_cast<Text *>(token);
			if(t != nullptr) {
				if (!mt.empty()) {
					auto back = mt.back();
					Text *ptr = dynamic_cast<Text *>(back.get());
					if (ptr != nullptr) {
						t->text = ptr->text + t->text;
						mt.pop_back();
					}
				}
			}
		}
		mt.emplace_back(token);
	}

}
