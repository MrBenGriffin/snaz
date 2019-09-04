//
// Created by Ben on 2019-01-23.
//

#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"

namespace mt {

	Injection Text::i = Injection("i");
	Injection Text::k = Injection("k");

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

	void Text::clone(MacroText &out) const {
		if(!text.empty()) {
			auto token=make_unique<Text>(this->text);
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

	void Text::interpolate(const string& str,const string& cutter,MacroText& result, Injection& inter) const {
		if (!cutter.empty()) {
			string basis = str;
			while (!basis.empty() ) {
				string::size_type pos = basis.find(cutter);
				if (pos != string::npos) {
					result.add(basis.substr(0, pos)); //pos says 2
					basis = basis.substr(pos+cutter.size(),string::npos);
					auto token=make_unique<Injection>(inter);
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
			auto& cutter = s.stuff[0].first;
			auto& iter = s.stuff[1].first;
			if (!cutter.empty()) {
				string basis = text;
				while (!basis.empty() ) {
					string::size_type pos = basis.find(cutter);
					if (pos != string::npos) {
						interpolate(basis.substr(0, pos),iter,result,Text::k);
						basis = basis.substr(pos+cutter.size(),string::npos);
						auto token=make_unique<Injection>(Text::i);
						result.emplace(token);
					} else {
						interpolate(basis,iter,result,Text::k);
						basis.clear();
					}
				}
			} else {
				interpolate(text,iter,result,Text::k);
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
