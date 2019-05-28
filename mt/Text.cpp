//
// Created by Ben on 2019-01-23.
//

#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"

namespace mt {

	Text::Text(const std::string& w) : Script(w) {}
	Text::Text(const std::ostringstream& s) : Script(s) {}

	//const std::string&
	std::ostream &Text::visit(std::ostream &o) const {
		o << "“" << text.str() << "”" << std::flush;
		return o;
	}

	void Text::expand(Messages& m,mtext &mt,mstack &) const {
		if(size > 0x1000) {
			assert(true);
		}
		Token::add(this,mt);
	}

	void Text::subs(mtext& result,const vector<string>& list,const string& prefix) const {
		size_t start = 0, curr, psize = prefix.size();
		string valStr;
		const string& txt = text.str();
		while ((curr=txt.find(prefix,start)) != string::npos) {
			valStr.append(txt.substr(start,curr - start)); //number of chars.
			curr += psize;
			pair<size_t,bool> idx = Support::znatural(txt,curr);
			if(idx.second) {
				if(idx.first < list.size()) {
					valStr.append(list[idx.first]);
				}
			} else {
				valStr.append(prefix);
			}
			start += curr;
		}
		if(start < txt.size()) {
			valStr.append(txt.substr(start));
		}
		Token::add(valStr,result);
	}

	void Text::doFor(mtext& result,const forStuff& s) const {
		string basis = text.str();
		Support::fandr(basis,s.stuff[0].first,s.stuff[0].second);
		Support::fandr(basis,s.stuff[1].first,s.stuff[1].second);
		Token::add(basis,result);
	}


}
