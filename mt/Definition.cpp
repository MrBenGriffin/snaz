#include <utility>

//
// Created by Ben on 2019-01-10.
//

#include <utility>
#include <variant>

#include "support/Message.h"
#include "support/Timing.h"

#include "support/db/Connection.h"
#include "support/db/Query.h"

#include "mt/mt.h"
#include "mt/Internal.h"
#include "mt/Internals.h"

namespace mt {

	std::unordered_map<std::string, Handler> Definition::library;


//Load from database.
	void Definition::load(Messages& errs,Db::Connection& sql,buildKind kind) {
		Timing& times = Timing::t();
		mt::Internal::startup(errs,sql,kind); //loading internals and storage?!;

		if (times.show()) { times.set("Load Site Macros"); }
		//Now load macros.
		if ( sql.dbselected() && sql.table_exists(errs,"bldmacro") && sql.table_exists(errs,"bldmacrov") ) {
			sql.lock(errs,"bldmacro as m read,bldmacrov as v read");
			std::ostringstream str;
			str << "select m.search,v.expansion,m.id,v.stripf='on' as strip_d,v.stripx='on' as strip_p,v.preparse='on' as pre_p,v.minparms,v.maxparms from bldmacro m,bldmacrov v where v.id=m.version and v.expansion !='internal'";
			Db::Query* q = nullptr;
			if (sql.query(errs,q,str.str()) && q->execute(errs)) {
//				+---------+-----------+----+---------+---------+-------+----------+----------+
//				| search  | expansion | id | strip_d | strip_p | pre_p | minparms | maxparms |
//				+---------+-----------+----+---------+---------+-------+----------+----------+
//				| wa      |  %1="%2"  | 66 |       1 |       1 |     0 |        2 |        2 |
//				+---------+-----------+----+---------+---------+-------+----------+----------+
				std::string name,expansion,macro_id;
				signed long min,max;
				size_t id,pre,stripDef,stripParms;
//				str.str(""); str << "Found " << q->getnumrows() << " macro Definitions";
//				errs << Message(debug,str.str());
				while(q->nextrow()) {
					q->readfield(errs,"search",name);
					q->readfield(errs,"expansion",expansion);
					q->readfield(errs,"id",macro_id);
					q->readfield(errs,"strip_d",stripDef);
					q->readfield(errs,"strip_p",stripParms);
					q->readfield(errs,"preparse",pre);
					q->readfield(errs,"minparms",min);
					q->readfield(errs,"maxparms",max);
					if (name.empty()) {
						errs << Message(error,"macro with id " + macro_id + " has no name!");
					} else {
						Messages log;
						Definition macro(log,name,expansion,min,max,stripDef==1,stripParms==1,pre==1);
						if(log.marked()) {
							str.str(""); str << "Define `" << name << "` failed while parsing `" << expansion << "`";
							errs << Message(error,str.str());
							errs += log;
						} else {
							add(macro);
						}
					}
				}
//				str.str(""); str << "Loaded " << library.size() << " Macro Definitions";
//				errs << Message(debug,str.str());
			}
			sql.dispose(q);
			sql.unlock(errs);
		}
		if (times.show()) { times.use(errs,"Load Site Macros"); }
	}

	void Definition::shutdown(Messages& errs,Db::Connection& sql,buildKind kind) {
		mt::Internal::shutdown(errs,sql,kind); //savePersistance();
	}

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
			expansion = result.second.first;
			iterated = result.second.second;
			if(! result.first ) {
				ostringstream str;
				str << "Macro `" << name_i <<  "` parse failed using expansion: `" << expansion_i << "`";
				errs << Message(error,str.str());
			}
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
			iteration* i = &(context.front().second.it); //so we are iterating front (because not all are generated).
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

	void Definition::startup(Messages& log) {
		Timing& times = Timing::t();
		if (times.show()) { times.set("Load Internal Macros"); }

//•------------  Utility macros
		library.emplace("iEq",Handler(std::move(iEq())));
		library.emplace("iIndex",Handler(std::move(iIndex())));
		library.emplace("iForIndex",Handler(std::move(iForIndex())));
		library.emplace("iExpr",Handler(std::move(iExpr())));
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

//•------------  Storage macros
		library.emplace("iExists",Handler(std::move(iExists())));
		library.emplace("iSet",Handler(std::move(iSet())));
		library.emplace("iGet",Handler(std::move(iGet())));
		library.emplace("iAppend",Handler(std::move(iAppend())));
		library.emplace("iKV",Handler(std::move(iKV())));
		library.emplace("iList",Handler(std::move(iList())));
		library.emplace("iReset",Handler(std::move(iReset())));
		library.emplace("iSetCache",Handler(std::move(iSetCache())));
		library.emplace("iSig",Handler(std::move(iSig())));
		library.emplace("iUse",Handler(std::move(iUse())));

//•------------  String macros
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

//•------------ Encoder macros
		library.emplace("iBase64",Handler(std::move(iBase64())));
		library.emplace("iDecode",Handler(std::move(iDecode())));
		library.emplace("iEncode",Handler(std::move(iEncode())));
		library.emplace("iHex",Handler(std::move(iHex())));
		library.emplace("iUnHex",Handler(std::move(iUnHex())));
		library.emplace("iUpper",Handler(std::move(iUpper())));
		library.emplace("iLower",Handler(std::move(iLower())));
		library.emplace("iUrlEncode",Handler(std::move(iUrlEncode())));
		library.emplace("iDigest",Handler(std::move(iDigest())));

//•------------ NodeTree
		library.emplace("iNumChildren",Handler(std::move(iNumChildren())));
		library.emplace("iNumGen",Handler(std::move(iNumGen())));
		library.emplace("iNumGens",Handler(std::move(iNumGens())));
		library.emplace("iNumPage",Handler(std::move(iNumPage())));
		library.emplace("iNumPages",Handler(std::move(iNumPages())));
		library.emplace("iNumSib",Handler(std::move(iNumSib())));
		library.emplace("iEqFamily",Handler(std::move(iEqFamily())));
		library.emplace("iEqNode",Handler(std::move(iEqNode())));
		library.emplace("iEqSibs",Handler(std::move(iEqSibs())));
		library.emplace("iExistNode",Handler(std::move(iExistNode())));
		library.emplace("iForAncestry",Handler(std::move(iForAncestry())));
		library.emplace("iForNodes",Handler(std::move(iForNodes())));
		library.emplace("iForPeers",Handler(std::move(iForPeers())));
		library.emplace("iForSibs",Handler(std::move(iForSibs())));
		library.emplace("iSize",Handler(std::move(iSize())));

//•------------ BuildAccessors
		library.emplace("iTech",Handler(std::move(iTech())));
		library.emplace("iPreview",Handler(std::move(iPreview())));
		library.emplace("iLang",Handler(std::move(iLang())));
		library.emplace("iLangID",Handler(std::move(iLangID())));
		library.emplace("iFullBuild",Handler(std::move(iFullBuild())));

//•------------ NodeAccessors
		library.emplace("iTitle",Handler(std::move(iTitle())));
		library.emplace("iTeam",Handler(std::move(iTeam())));
		library.emplace("iSuffix",Handler(std::move(iSuffix())));
		library.emplace("iShortTitle",Handler(std::move(iShortTitle())));
		library.emplace("iSegmentName",Handler(std::move(iSegmentName())));
		library.emplace("iLayout",Handler(std::move(iLayout())));
		library.emplace("iLayoutName",Handler(std::move(iLayoutName())));
		library.emplace("iLink",Handler(std::move(iLink())));
		library.emplace("iLinkRef",Handler(std::move(iLinkRef())));
		library.emplace("iID",Handler(std::move(iID())));
		library.emplace("iBirth",Handler(std::move(iBirth())));
		library.emplace("iContent",Handler(std::move(iContent())));
		library.emplace("iDeath",Handler(std::move(iDeath())));
		library.emplace("iExistContent",Handler(std::move(iExistContent())));
		library.emplace("iTW",Handler(std::move(iTW())));

//•------------ Media
		library.emplace("iMedia",Handler(std::move(iMedia())));
		library.emplace("iEmbed",Handler(std::move(iEmbed())));
		library.emplace("iExistMedia",Handler(std::move(iExistMedia())));

//•------------ Taxonomy
		library.emplace("iTax",Handler(std::move(iTax())));
		library.emplace("iForTax",Handler(std::move(iForTax())));
		library.emplace("iForTaxNodes",Handler(std::move(iForTaxNodes())));
		library.emplace("iExistSimilar",Handler(std::move(iExistSimilar())));
		library.emplace("iForSimilar",Handler(std::move(iForSimilar())));


//•------------ FINISHED
		if (times.show()) { times.use(log,"Load Internal Macros"); }

	}

}
