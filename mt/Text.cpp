//
// Created by Ben on 2019-01-23.
//

#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"

namespace mt {

	Text::Text(std::string w) : Script(std::move(w)) {}

	Text::Text(const Text* o) : Script(o->text) {
		if(o->text.empty()) {
			throw bad_alloc();
		}
	}

	std::ostream &Text::visit(std::ostream &o) const {
		o << "“" << text << "”" << std::flush;
		return o;
	}

	void Text::expand(Messages&,MacroText &mt,mstack &) const {
		if(!text.empty()) {
			auto token=make_unique<Text>(this->text);
			mt.emplace(token);
		}
	}
	
	unique_ptr<Token> Text::clone() const {
		std::unique_ptr<Token>derived = std::make_unique<Text>(this);
		return derived;
	}

	void Text::subs(const vector<string>& list,const string& prefix) {
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
		swap(text,valStr);
	}

	void Text::doFor(MacroText& result,const forStuff& s) const {
		string basis(text);
		Support::fandr(basis,s.stuff[0].first,s.stuff[0].second);
		Support::fandr(basis,s.stuff[1].first,s.stuff[1].second);

		if(!basis.empty()) {
			auto token=make_unique<Text>((basis));
			result.emplace(token);
		}
	}

	void Text::inject(Messages&,MacroText& out,mstack&) const {
		if(!text.empty()) {
			auto token=make_unique<Text>(this->text);
			out.emplace(token);
		}
	}

}
