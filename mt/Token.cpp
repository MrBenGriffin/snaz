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

	void Token::add(std::string text,mtext& mt) {
		if (!mt.empty()) {
			auto back = mt.back();
			Text *ptr = dynamic_cast<Text *>(back.get());
			if (ptr != nullptr) {
				ptr->text.append(text);
			} else {
				mt.emplace_back(make_shared<Text>(text));
			}
		} else {
			mt.emplace_back(make_shared<Text>(text));
		}
	}

	void Token::add(TokenPtr& token,mtext& mt) {
		if (token != nullptr) {
			Text *t = dynamic_cast<Text *>(token.get());
			if (t != nullptr) {
				if (!mt.empty()) {
					auto back = mt.back();
					Text *ptr = dynamic_cast<Text *>(back.get());
					if (ptr != nullptr) {
						t->text.swap(ptr->text);
						t->text.append(ptr->text);
						mt.pop_back();
					}
				}
			} else {
				Wss *w = dynamic_cast<Wss *>(token.get());
				if (w != nullptr) {
					if (!mt.empty()) {
						auto back = mt.back();
						Wss *ptr = dynamic_cast<Wss *>(back.get());
						if (ptr != nullptr) {
							w->text.swap(ptr->text);
							w->text.append(ptr->text);
							mt.pop_back();
						}
					}
				}
			}
			mt.emplace_back(token);
		}
	}
}