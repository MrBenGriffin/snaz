//
// Created by Ben on 2019-01-23.
//

#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"

namespace mt {

	Text::Text(const std::string &w) : text(w) {}

	std::ostream &Text::visit(std::ostream &o) const {
		o << "“" << text << "”" << std::flush;
		return o;
	}

	std::string Text::get() const { return text; }
	void Text::append(std::string right) { text.append(right); };

	void Text::expand(Messages& m,mtext &mt,const mstack &context) const {
		string result(text);
		if (!mt.empty()) {
			if (std::holds_alternative<Text>(mt.back())) {
				std::get<Text>(mt.back()).text.append(result);
			} else {
				if (std::holds_alternative<Wss>(mt.back())) {
					std::string ws = std::get<Wss>(mt.back()).get();
					mt.pop_back();
					ws.append(result);
					mt.emplace_back(std::move(Text(ws)));
				} else {
					mt.emplace_back(Text(result));
				}
			}
		} else {
			mt.emplace_back(Text(result));
		}
	}

	void Text::add(mtext &mt) {
		if (!mt.empty() && std::holds_alternative<Text>(mt.back())) {
			text = std::get<Text>(mt.back()).text + text;
			mt.pop_back();
		}
		mt.emplace_back(std::move(*this));
	}

	void Text::subs(mtext& result,const vector<string>& list,const string prefix) const {
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
		Text(valStr).add(result);
	}

	void Text::doFor(mtext& result,const forStuff& stuff) const {
		string basis(text);
		Support::fandr(basis,stuff.stuff);
		Text(basis).add(result);
	}

}