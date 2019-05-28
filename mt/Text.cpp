//
// Created by Ben on 2019-01-23.
//

#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"

namespace mt {

	Text::Text(std::string w) : Script(std::move(w)) {}

	std::ostream &Text::visit(std::ostream &o) const {
		o << "“" << text << "”" << std::flush;
		return o;
	}

	void Text::expand(Messages& m,mtext &mt,mstack &) const {
//		if(text.size() > 0x100000) {
//			assert(true);
//		}
		if (!mt.empty()) {
			Token *back = mt.back().get();
			auto *textPtr = dynamic_cast<Text *>(back);
			if (textPtr != nullptr) {
				size_t newSize = textPtr->text.size() + text.size();
				textPtr->text.reserve(newSize);
				textPtr->text.append(text);
			} else {
				Wss *wss = dynamic_cast<Wss *>(back);
				if (wss != nullptr) {
					std::string ws = wss->get();
					mt.pop_back();
					ws.append(text);
					shared_ptr<Token> ptr = make_shared<Text>(ws);
					mt.emplace_back(ptr);
				} else {
					shared_ptr<Token> ptr = make_shared<Text>(text);
					mt.emplace_back(ptr);
				}
			}
		} else {
			Token::add(text,mt);
		}
	}

	void Text::subs(mtext& result,const vector<string>& list,const string& prefix) const {
		size_t start = 0, curr, psize = prefix.size();
		string valStr;
		while ((curr=text.find(prefix,start)) != string::npos) {
			valStr.append(text.substr(start,curr - start)); //number of chars.
			curr += psize;
			pair<size_t,bool> idx = Support::znatural(text,curr);
			if(idx.second) {
				if(idx.first < list.size()) {
					valStr.append(list[idx.first]);
				}
			} else {
				valStr.append(prefix);
			}
			start += curr;
		}
		if(start < text.size()) {
			valStr.append(text.substr(start));
		}
		Token::add(valStr,result);
	}

	void Text::doFor(mtext& result,const forStuff& s) const {
		string basis(text);
		Support::fandr(basis,s.stuff[0].first,s.stuff[0].second);
		Support::fandr(basis,s.stuff[1].first,s.stuff[1].second);
		Token::add(basis,result);
	}

	void Text::inject(Messages&,mtext& out,mstack&) const {
		Token::add(text,out);
	}


}
