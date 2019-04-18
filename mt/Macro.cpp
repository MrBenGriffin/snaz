//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
#include "Definition.h"

namespace mt {

	Macro::Macro(std::string n) : name(std::move(n)) {};

	void Macro::inject(Messages& e,mtext& result,mstack &context) const {
		Macro ret(name);
		for(auto& parm : parms) {
			mtext nParm;
			Driver::inject(parm,e,nParm,context);
			ret.parms.push_back(nParm);
		}
		result.push_back({std::move(ret)});
	}

	void Macro::subs(mtext& result,std::vector<std::string>& subs,const std::string& prefix) const {
		Macro ret(name);
		for(auto& parm : parms) {
			mtext nParm;
			Driver::subs(parm,nParm,subs,prefix);
			ret.parms.push_back(nParm);
		}
		result.push_back({std::move(ret)});
	}

	void Macro::doFor(mtext& result,const forStuff& stuff) const {
		Macro ret(name);
		for(auto& parm : parms) {
			mtext nParm;
			Driver::doFor(parm,nParm,stuff);
			ret.parms.push_back(nParm);
		}
		result.push_back({std::move(ret)});
	}


	void Macro::expand(Messages& errs,mtext& result,mstack &context) const {
		if (Definition::has(name)) {
			Instance instance(&parms,{0,parms.size()});
			Definition::exp(name,errs,result,instance,context);
		} else {
			result.emplace_back(*this);
		}
	}

	std::ostream &Macro::visit(std::ostream &result) const {
		result << "⸠" << name;
		for (auto &p : parms) {
			result << "「";
			if (!p.empty()) {
				Driver::visit(p, result);
			}
			result << "」";
		}
		return result;
	}

	void Macro::add(mtext& mt) {
		mt.emplace_back(std::move(*this));
	}

}

