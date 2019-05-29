//
// Created by Ben on 2019-01-23.
//

#include "mt/mt.h"
#include "mt/Definition.h"
#include "mt/MacroText.h"

namespace mt {

	Macro::Macro(std::string n) : _name(std::move(n)) {};

	Macro::Macro(std::string n,plist p) : _name(std::move(n)),parms(std::move(p)) {
		while (!parms.empty() && parms.back().empty()) {
			parms.pop_back();
		}
	}

	void Macro::final(std::ostream& o) const {
		visit(o);
	}

	std::string Macro::get() const {
		return _name; //not sure this is correct.
	}

	bool Macro::empty() const {
		return false;
	}
	
	Macro::Macro(const Macro* other) : _name(other->_name) {
		for(auto& parm : other->parms) {
			MacroText nParm;
			nParm.add(parm);
			parms.emplace_back(move(nParm));
		}
	}
	
	unique_ptr<Token> Macro::clone() const {
		std::unique_ptr<Token>derived = std::make_unique<Macro>(this);
		return derived;
	}

	void Macro::doFor(MacroText& result,const forStuff& stuff) const {
		auto token=make_unique<Macro>(_name);
		for(auto& parm : parms) {
			MacroText nParm;
			parm.doFor(nParm,stuff);
			token->parms.emplace_back(move(nParm));
		}
		result.emplace(token);
	}

	void Macro::inject(Messages& e,MacroText& result,mstack &context) const {
		auto token=make_unique<Macro>(_name);
		for(auto& parm : parms) {
			MacroText nParm;
			parm.inject(e,nParm,context);
			token->parms.emplace_back(move(nParm));
		}
		result.emplace(token);
	}

	void Macro::subs(const std::vector<std::string>& subs,const std::string& prefix) {
		for(auto& parm : parms) {
			parm.subs(subs,prefix);
		}
	}

	void Macro::expand(Messages& errs,MacroText& result,mstack &context) const {
		auto librarian = Definition::library.find(_name);
		if(librarian != Definition::library.end()) {
			try {
				// Expand passes by reference. Instance& (P5)
				Instance instance(&parms,context.back().second->metrics);
				librarian->second->expand(errs,result,instance,context);
			} catch (...) {}
		} else {
			ostringstream err;
			err << "The macro instance '" << flush;
			visit(err);
			err << flush << "' was not recognised. It will be skipped.";
			errs << Message(error,err.str());
		}
	}

	std::ostream &Macro::visit(std::ostream &result) const {
		result << "⸠" << _name;
		for (auto &p : parms) {
			result << "「";
			p.visit(result);
			result << "」";
		}
		return result;
	}

}

