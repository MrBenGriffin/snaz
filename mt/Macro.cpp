//
// Created by Ben on 2019-01-23.
//

#include "mt.h"
#include "Definition.h"

namespace mt {

	Macro::Macro(std::string n) : _name(std::move(n)) {};

	Macro::Macro(std::string n,plist p) : _name(std::move(n)),parms(std::move(p)) {}

	void Macro::final(std::ostream& o) const {
		visit(o);
	}

	std::string Macro::get() const {
		return _name; //not sure this is correct.
	}

	bool Macro::empty() const {
		return false;
	}

	void Macro::doFor(mtext& result,const forStuff& stuff) const {
		auto ret = make_shared<Macro>(_name);
		for(auto& parm : parms) {
			mtext nParm;
			Driver::doFor(parm,nParm,stuff);
			ret.get()->parms.emplace_back(nParm);
		}
		result.emplace_back(ret);
	}

	void Macro::inject(Messages& e,mtext& result,mstack &context) const {
		auto ret = make_shared<Macro>(_name);
		for(auto& parm : parms) {
			mtext nParm;
			Driver::inject(parm,e,nParm,context);
			ret.get()->parms.emplace_back(nParm);
		}
		result.emplace_back(ret);
	}

	void Macro::subs(mtext& result,const std::vector<std::string>& subs,const std::string& prefix) const {
		auto ret = make_shared<Macro>(_name);
		for(auto& parm : parms) {
			mtext nParm;
			Driver::subs(parm,nParm,subs,prefix);
			ret.get()->parms.emplace_back(nParm);
		}
		result.emplace_back(ret);
	}

	void Macro::expand(Messages& errs,mtext& result,mstack &context) const {
		auto good = Definition::library.find(_name);
		if(good != Definition::library.end()) {
			iteration iter({0,parms.size()});
			Instance instance(parms,iter,context.back().second.metrics);
			auto& macro = good->second;
			try {
				macro->expand(errs,result,instance,context);
			} catch (...) {}
		} else {
			auto macro = make_shared<Macro>(_name,parms);
			result.emplace_back(macro); //Because the method is const, 'this' is a const*
			errs << Message(error,_name + " not found.");
		}
	}

	std::ostream &Macro::visit(std::ostream &result) const {
		result << "⸠" << _name;
		for (auto &p : parms) {
			result << "「";
			if (!p.empty()) {
				Driver::visit(p, result);
			}
			result << "」";
		}
		return result;
	}

}

