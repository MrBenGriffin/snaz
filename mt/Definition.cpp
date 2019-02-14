#include <utility>

//
// Created by Ben on 2019-01-10.
//

#include <utility>
#include <variant>
#include "mt.h"
#include "Internal.h"
#include "Internals.h"

namespace mt {

	std::unordered_map<std::string, Handler> Definition::library;
//	Definition Definition::empty(Messages(),"__empty", "", false, false, false);

	Definition::Definition(
			Messages& errs,std::string name_i, std::string expansion_i,
			long min, long max, bool strip, bool trimParms_i, bool preExpand_i)
			: counter(0), _name(std::move(name_i)), trimParms(trimParms_i), preExpand(preExpand_i) {
		minParms = min == -1 ? 0 : min;
		maxParms = max == -1 ? INT_MAX : max;
		if(expansion_i.empty()) {
			expansion = {};
			iterated = false;
		} else {
			std::istringstream code(expansion_i);
			bool advanced = test_adv(expansion_i);
			Driver driver(errs,code,advanced);
			parse_result result = driver.define(errs,strip);
			expansion = result.first;
			iterated = result.second;
		}
	}

	//used for pre-parsed definitions (eg in iForSibs)
	Definition::Definition(const mtext code,
		long min, long max, bool iter, bool tP, bool preEx):
		counter(0), _name(":internal:"),iterated(iter), trimParms(tP), preExpand(preEx),expansion(code) {
		minParms = min == -1 ? 0 : min;
		maxParms = max == -1 ? INT_MAX : max;
	}

	bool Definition::inRange(size_t i) const {
		return (i <= maxParms);
	}

	bool Definition::parmCheck(Support::Messages& e,size_t parmcount) const {
		if((minParms <= parmcount) && (parmcount <= maxParms)) {
			return true;
		}
		std::ostringstream errstr;
		errstr << "Range error. '" << _name << "' uses ";
		if (maxParms == 2147483647) {
			errstr << "at least " << minParms;
			errstr << " paramete" << (minParms == 1 ? "r" : "rs");
		} else {
			if(minParms == maxParms) {
				errstr << "exactly " << minParms;
				errstr << " paramete" << (minParms == 1 ? "r" : "rs");
			} else {
				errstr << minParms << " to " << maxParms << " parameters";
			}
		}
		errstr << " but " << parmcount << " were found.";
		e << Message(range,errstr.str());
		return false;
	}


	void Definition::expand(Support::Messages& e,mtext& o,Instance &instance, mstack &context) {
		//Range error. 'pt' uses exactly 1 parameter but 0 were found.
		size_t pCount = instance.parms->size() == 1 ? instance.parms->front().empty() ? 0 : 1 : instance.parms->size();
		if(!parmCheck(e,pCount)) {
			return;
		}
		Instance modified(instance);
		plist rendered;
		if (!preExpand) {
			plist mt_parms = *instance.parms;
			if (trimParms) { trim(mt_parms); }
			modified.parms = &rendered;
			modified.generated = false;
			context.push_back({nullptr,modified}); //This is done for injections like %(1+).
			for (auto &parm : mt_parms) {
				mtext expanded;
				auto i=rendered.size();
				Driver::expand(parm,e,expanded, context);
				if(rendered.size() == i) {
					rendered.push_back(std::move(expanded)); //each one as a separate parm!!
				}
			}
			context.pop_back();
			modified.generated = instance.generated;
			while (!rendered.empty() && rendered.back().empty()) {
				rendered.pop_back();
			}
			modified.it = {0,rendered.size()};
		}
		Handler handler(*this);
		context.push_front({&handler,modified});
		if (iterated) {
			if(modified.generated) { context.push_back({nullptr,modified}); }
			iteration* i = &(context.front().second.it);
			for (i->first = 1; i->first <= i->second; i->first++) {
				Driver::expand(expansion,e, o, context);
			}
			if(modified.generated) {
				context.pop_back();
			}
		} else {
			Driver::expand(expansion,e, o, context);
		}
		context.pop_front();
	}


	bool Definition::test_adv(std::string &basis) {
		//⌽E2.8C.BD ⍟E2.8D.9F ⎣E2.8E.A3 ⎡e2.8e.A1 	/** ALL START E2. (⌽ 8c bd)(⍟ 8d 9F)(⎡8e A1)(⎤ 8e A4)(⎣ 8e A3)(❫ 9D AB) **/
		if (!basis.empty()) {
			size_t size = basis.size();
			size_t offset = basis.find(0xE2);
			while(offset != std::string::npos && offset + 2 < size) {
				char a = basis[offset + 1];
				char b = basis[offset + 2];
				if (a == char(0x8C) && b == char(0xBD)) return true; // ⌽
				if (a == char(0x8D) && b == char(0x9F)) return true; // ⍟
				if (a == char(0x8E) && (b == char(0xA1) || b == char(0xA3))) return true; //⎡  or  ⎣
				offset = basis.find(0xE2, offset+2);
			}
		}
		return false;
	}

	void Definition::vis(std::string name,std::ostream& o) {
		auto good = Definition::library.find(name);
		if(good != Definition::library.end()) {
			if (std::holds_alternative<Definition>(good->second)) {
				std::get<Definition>(good->second).visit(o);
			} else {
				o << "«" << name << "»";
			}
		}
	}

	void Definition::exp(std::string name,Messages& e,mtext& o,Instance& i,mt::mstack& c) {
		auto good = Definition::library.find(name);
		if(good != Definition::library.end()) {
			std::visit([&e,&o,&i,&c](auto&& arg){ arg.expand(e,o,i,c);},good->second);
		} else {
			e << Message(error,name + " not found.");
		}
	}

	void Definition::add(Definition& macro) {
		del(macro.name());
		std::string name(macro.name());
		library.emplace(macro.name(),Handler(std::move(macro)));
	}

	bool Definition::has(std::string name) {
		return library.find(name) != Definition::library.end();
	}

	void Definition::del(std::string name) {
		auto good = library.find(name);
		if (good != library.end()) {
			library.erase(good);
		}
	}

	void Definition::list(std::ostream& o) {
		for (auto& a :library) {
			o << a.first << ": ";
			vis(a.first,o);
			o << std::endl;
		}
	}


	std::ostream &Definition::visit(std::ostream &o) {
		return Driver::visit(expansion, o);
	}

	void Definition::trim(plist &bobs) {
		for (auto &j : bobs) { //each parameter.
			while (!j.empty() && std::holds_alternative<Wss>(j.front())) {
				j.pop_front();
			}
			while (!j.empty() && std::holds_alternative<Wss>(j.back())) {
				j.pop_back();
			}
		}
	}

	void Definition::startup() {
		library.emplace("iEq",Handler(std::move(iEq())));
		library.emplace("iExpr",Handler(std::move(iExpr())));
		library.emplace("iIndex",Handler(std::move(iIndex())));
		library.emplace("iForIndex",Handler(std::move(iForIndex())));

		library.emplace("iConsole",Handler(std::move(iConsole())));
		library.emplace("iDate",Handler(std::move(iDate())));
		library.emplace("iEval",Handler(std::move(iEval())));
		library.emplace("iFile",Handler(std::move(iFile())));
		library.emplace("iField",Handler(std::move(iField())));
		library.emplace("iForSubs",Handler(std::move(iForSubs())));
		library.emplace("iForQuery",Handler(std::move(iForQuery())));
		library.emplace("iMath",Handler(std::move(iMath())));
		library.emplace("iNull",Handler(std::move(iNull())));
		library.emplace("iTiming",Handler(std::move(iTiming())));

		library.emplace("iGet",Handler(std::move(iGet())));
		library.emplace("iSet",Handler(std::move(iSet())));
		library.emplace("iExists",Handler(std::move(iExists())));
		library.emplace("iAppend",Handler(std::move(iAppend())));
		library.emplace("iKV",Handler(std::move(iKV())));
		library.emplace("iList",Handler(std::move(iList())));
		library.emplace("iReset",Handler(std::move(iReset())));
		library.emplace("iSetCache",Handler(std::move(iSetCache())));
		library.emplace("iSig",Handler(std::move(iSig())));
		library.emplace("iUse",Handler(std::move(iUse())));

		library.emplace("iLeft",Handler(std::move(iLeft())));
		library.emplace("iLength",Handler(std::move(iLength())));
		library.emplace("iMid",Handler(std::move(iMid())));
		library.emplace("iPosition",Handler(std::move(iPosition())));
		library.emplace("iRegex",Handler(std::move(iRegex())));
		library.emplace("iRembr",Handler(std::move(iRembr())));
		library.emplace("iRembrp",Handler(std::move(iRembrp())));
		library.emplace("iReplace",Handler(std::move(iReplace())));
		library.emplace("iRight",Handler(std::move(iRight())));
		library.emplace("iTrim",Handler(std::move(iTrim())));

	}

}
