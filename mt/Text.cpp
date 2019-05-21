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

	void Text::expand(Messages& m,mtext &mt,mstack &context) const {
		if (!mt.empty()) {
			Token *back = mt.back().get();
			auto *textPtr = dynamic_cast<Text *>(back);
			if (textPtr != nullptr) {
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
			shared_ptr<Token> ptr = make_shared<Text>(text);
			mt.emplace_back(ptr);
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
		shared_ptr<Token> ptr = make_shared<Text>(valStr);
		add(ptr,result);
	}

	void Text::doFor(mtext& result,const forStuff& stuff) const {
		string basis(text);
		Support::fandr(basis,stuff.stuff);
		shared_ptr<Token> ptr = make_shared<Text>(basis);
		add(ptr,result);
	}

	void Text::inject(Messages&,mtext& out,mstack&) const {
		shared_ptr<Token> ptr = make_shared<Text>(text);
		out.emplace_back(ptr);
	}


}