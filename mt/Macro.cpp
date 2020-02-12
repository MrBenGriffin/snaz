//
// Created by Ben on 2019-01-23.
//

#include <iomanip>
#include "Build.h"
#include "mt/mt.h"
#include "mt/Definition.h"
#include "mt/MacroText.h"

namespace mt {

	Macro::Macro(std::string n) : _name(std::move(n)) {};

	Macro::Macro(std::string w,location& pos) : Token(pos) {
		swap(_name,w);
	}

	Macro::Macro(std::string n,plist p) : _name(std::move(n)),parms(std::move(p)) {
		while (!parms.empty() && parms.back().empty()) {
			parms.pop_back();
		}
	}

	void Macro::final(std::ostream& o) const {
		visit(o, 0);
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
	void Macro::addParms( size_t count) { //add empty parms.
		for (size_t i=0; i < count; ++i) {
			parms.emplace_back(MacroText());
		}
	}
	void Macro::addParm(unique_ptr<Wss> token) {
		MacroText mt; mt.emplace(token);
		parms.emplace_back(move(mt));
	}
	void Macro::addParm(unique_ptr<Text> token) {
		MacroText mt; mt.emplace(token);
		parms.emplace_back(move(mt));
	}
	void Macro::addParm(unique_ptr<Macro> token) {
		MacroText mt; mt.emplace(token);
		parms.emplace_back(move(mt));
	}
	void Macro::addParm(unique_ptr<Injection> token) {
		MacroText mt; mt.emplace(token);
		parms.emplace_back(move(mt));
	}

	void Macro::clone(MacroText &out) const {
		auto token= make_unique<Macro>(this);
		token->pos = pos;
		out.emplace(token);
	}

	void Macro::doFor(Messages& e,MacroText& result,const forStuff& stuff,mstack& c) const {
		auto token=make_unique<Macro>(_name);
		for(auto& parm : parms) {
			MacroText nParm;
			parm.doFor(e,nParm,stuff,c);
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

	void Macro::subs(MacroText& out,const std::deque<std::string>& subs,const std::string& prefix) const {
		auto token=make_unique<Macro>(_name);
		for(auto& parm : parms) {
			MacroText nParm;
			parm.subs(nParm,subs,prefix);
			token->parms.emplace_back(move(nParm));
		}
		out.emplace(token);
	}

	void Macro::check(Messages &errs, mstack& context) const {
		auto librarian = Definition::library.find(_name);
		if(librarian != Definition::library.end()) {
			try {
				// Expand passes by reference. Instance& (P5)
				if(!context.empty()) {
					Instance instance(&parms,context.back().second->metrics);
					librarian->second->check(errs,instance,pos);
				} else {
					Instance instance(&parms, nullptr);
					librarian->second->check(errs,instance,pos);
				}
			} catch (...) {}
		} else {
			ostringstream err;
			err << "Macro '" << _name << "' is undefined." << flush;
			errs << Message(undefined, err.str(), pos);
			Instance instance(&parms, nullptr);
			for (auto &parm : instance.parms) {
				parm.check(errs,context);
			}
		}
	}

	void Macro::expand(Messages& errs,MacroText& result,mstack &context) const {
		auto librarian = Definition::library.find(_name);
		if(librarian != Definition::library.end()) {
			try {
				// Expand passes by reference. Instance& (P5)
				if(!context.empty()) {
					Instance instance(&parms,context.back().second->metrics);
					librarian->second->expand(errs,result,instance,context);
				} else {
					Instance instance(&parms, nullptr);
					librarian->second->expand(errs,result,instance,context);
				}
			} catch (...) {}
		} else {
			ostringstream err;
			err << "Macro '" << flush;
			visit(err, 0);
			err << flush << "' was not recognised. It will be skipped.";
			errs << Message(undefined,err.str());
		}
	}

	std::ostream &Macro::visit(std::ostream &result, int style) const {
		switch(style) {
			case 0: result << "⸠" << _name; break;
			case 1: result << "⌽" << _name; break;
			case 2: result << "@" << _name; break;
			case 3: result << "@" << setfill('m') << setw((int)_name.size()) << ""; break;
		}
		string split = "(";
		for (auto &p : parms) {
			result << split;
			p.visit(result, style);
			switch(style) {
				case 0: split = "」"; break;
				case 1:
				case 2:
				case 3: split = ","; break;
			}
		}
		result << ")";
		return result;
	}

}

