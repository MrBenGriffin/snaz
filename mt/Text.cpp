//
// Created by Ben on 2019-01-23.
//

#include <iomanip>
#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"

namespace mt {
//	static Injection i,k;
//	Injection Text::i = Injection("i");
//	Injection Text::k = Injection("k");
 Text::Text() : Script("") {}

	Text::Text(std::string w,location& pos) : Script(std::move(w), pos) {}

	Text::Text(std::string w) : Script(std::move(w)) {}

	Text::Text(const Text* o) : Script(o->text) {
		if(o->text.empty()) {
			throw bad_alloc();
		}
	}

	std::ostream& Text::visit(std::ostream& o, int style) const {
		switch(style) {
			case 0: o << "“" << text << "”"; break;
			case 1: o << text; break;
			case 2: o << text; break;
			case 3: o << setfill('t') << setw((int)text.size()) << ""; break;
		}
		return o;
	}

	void Text::expand(Messages&,MacroText &mt,mstack &) const {
		if(!text.empty()) {
			auto token=make_unique<Text>(this->text);
			mt.emplace(token);
		}
	}

	void Text::clone(MacroText &out) const {
		if(!text.empty()) {
			auto token=make_unique<Text>(this->text);
			token->pos = pos;
			out.emplace(token);
		}
	}

	void Text::subs(MacroText& o,const deque<string>& list,const string& prefix) const {
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
			start = curr;
		}
		if(start < text.size()) {
			valStr.append(text.substr(start));
		}
		o.add(valStr);
	}

	void Text::interpolate(const string& str,MacroText& result,const forStuff& s) const {
		if (!s.iterSym.empty()) {
			string basis = str;
			while (!basis.empty() ) {
				string::size_type pos = basis.find(s.iterSym);
				if (pos != string::npos) {
					result.add(basis.substr(0, pos)); //pos says 2
					basis = basis.substr(pos+s.iterSym.size(),string::npos);
					auto token=make_unique<Injection>(s.iterInj);
					result.emplace(token);
				} else {
					result.add(basis);
					basis.clear();
				}
			}
		} else {
			result.add(str);
		}
	}

	void Text::doFor(Messages&,MacroText& result,const forStuff& s,mstack&) const {
		if (!text.empty()) {
			if (!s.nodeSym.empty()) {
				string basis = text;
				while (!basis.empty() ) {
					string::size_type pos = basis.find(s.nodeSym);
					if (pos != string::npos) {
						interpolate(basis.substr(0, pos),result,s);
						basis = basis.substr(pos+s.nodeSym.size(),string::npos);
						auto token=make_unique<Injection>(s.nodeInj);
						result.emplace(token);
					} else {
						interpolate(basis,result,s);
						basis.clear();
					}
				}
			} else {
				interpolate(text,result,s);
			}
		}
	}

	void Text::inject(Messages&,MacroText& out,mstack&) const {
		if(!text.empty()) {
			auto token=make_unique<Text>(this->text);
			out.emplace(token);
		}
	}

}
