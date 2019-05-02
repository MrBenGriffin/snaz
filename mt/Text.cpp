//
// Created by Ben on 2019-01-23.
//

#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"

namespace mt {

	Text::Text(std::string w) : text(std::move(w)) {}

	bool Text::empty() const { return text.empty(); }

	std::ostream &Text::visit(std::ostream &o) const {
		o << "“" << text << "”" << std::flush;
		return o;
	}

	void Text::final(std::ostream& o) const {
		o << text;
	};

	std::string Text::get() const { return text; }

	void Text::append(std::string right) { text.append(std::move(right)); };

	void Text::expand(Messages& m,mtext &mt,mstack &context) const {
		if (!mt.empty()) {
			Token *back = mt.back().get();
			Text *textPtr = dynamic_cast<Text *>(back);
			if (textPtr != nullptr) {
				textPtr->text.append(text);
			} else {
				Wss *wss = dynamic_cast<Wss *>(back);
				if (wss != nullptr) {
					std::string ws = wss->get();
					mt.pop_back();
					ws.append(text);
					mt.emplace_back(new Text(ws));
				} else {
					mt.emplace_back(new Text(text));
				}
			}
		} else {
			mt.emplace_back(new Text(text));
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
		add(new Text(valStr),result);
	}

	void Text::doFor(mtext& result,const forStuff& stuff) const {
		string basis(text);
		Support::fandr(basis,stuff.stuff);
		add(new Text(basis),result);
	}

	void Text::inject(Messages&,mtext& out,mstack&) const {
		out.emplace_back(new Text(text));
	}

//	void Token::subs(mtext& out,const std::vector<std::string>&,const std::string&) const {
////-->		out.emplace_back(new <Token*>(this));
//	}
//	void Token::inject(Messages&,mtext& out,mstack&) const {
////-->		out.emplace_back(const_cast<Token*>(this));
//	}

}