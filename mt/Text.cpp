//
// Created by Ben on 2019-01-23.
//

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
		if(!context.empty() && context.back().second.generated) {
			doCountAndValue(m,mt,context);
		} else {
			if (!mt.empty()) {
				if (std::holds_alternative<Text>(mt.back())) {
					std::get<Text>(mt.back()).text.append(text);
				} else {
					if (std::holds_alternative<Wss>(mt.back())) {
						std::string ws = std::get<Wss>(mt.back()).get();
						mt.pop_back();
						ws.append(text);
						mt.emplace_back(std::move(Text(ws)));
					} else {
						mt.emplace_back(*this);
					}
				}
			} else {
				mt.emplace_back(*this);
			}
		}
	}

	void Text::add(mtext &mt) {
		if (!mt.empty() && std::holds_alternative<Text>(mt.back())) {
			text = std::get<Text>(mt.back()).text + text;
			mt.pop_back();
		}
		mt.emplace_back(std::move(*this));
	}

	void Text::doCount(mtext& result,const std::string& marker,size_t value,std::string& basis) const {
		size_t start = 0, curr, cSize = marker.size();
		string valStr;
		while ((curr=basis.find (marker,start)) != string::npos) {
			string txt = basis.substr (start, curr - start);
			start = curr + cSize;
			if(!txt.empty()) { result.push_back(Text(txt)); }		//store the left-string (if it's not empty).
			if(valStr.empty()) {
				std::ostringstream ost; ost << value; //parm by number.
				valStr = ost.str();
			}
			Text(valStr).add(result);
		}
		string txt = basis.substr(start);
		if(!txt.empty()) {
			Text(txt).add(result);
		}
	}

	void Text::doCountAndValue(Messages& m,mtext& result,const mstack &context) const {
		const Instance& instance = context.back().second;
		const std::string& value=instance.iValue;
		size_t start = 0,curr,vSize = value.size();
		size_t current = context.front().second.it.first;
		//-Yes, this doesn't really make sense..
		//-Basically, back is being used to keep record of the current expansion owner, but front holds the iteration counter...

		while ((curr=text.find(value,start)) != string::npos) {
			string txt = text.substr (start, curr - start);
			start = curr + vSize;
			if(!txt.empty()) {
				if (!instance.iCount.empty()) {
					doCount(result, instance.iCount, current, txt);
				} else {
					Text(txt).add(result);
				}
			}
			const mtext& parm = (*instance.parms)[current - 1];
			const_cast<mstack &>(context).back().second.generated=false;
			Driver::expand(parm,m,result,const_cast<mstack &>(context));
			const_cast<mstack &>(context).back().second.generated=true;
		}
		string txt = text.substr(start);
		if(!txt.empty()) {
			if (!instance.iCount.empty()) {
				doCount(result, instance.iCount, current, txt);
			} else {
				Text(txt).add(result);
			}
		}
	}
}