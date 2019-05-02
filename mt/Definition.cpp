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

	std::unordered_map<std::string, std::unique_ptr<const Handler> > Definition::library;

	std::string Definition::name() const {
		return _name;
	}

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
			if (sql.query(errs,q,str.str()) && q != nullptr && q->execute(errs)) {
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
						Definition* macro = new Definition(log,name,expansion,min,max,stripDef==1,stripParms==1,pre==1);
						if(log.marked()) {
							str.str(""); str << "Define `" << name << "` failed while parsing `" << expansion << "`";
							errs << Message(error,str.str());
							errs += log;
						} else {
							del(name);
							library.emplace(name,macro);
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
			: Handler(), counter(0), _name(std::move(name_i)), trimParms(trimParms_i), preExpand(preExpand_i) {
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
	Definition::Definition(const mtext& code,
		long min, long max, bool iter, bool tP, bool preEx): Handler(),
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

	void Definition::expand(Support::Messages& e,mtext& o,Instance &instance, mstack &context) const {
		if(!parmCheck(e,instance.size())) {
			return;
		}
		Instance modified(instance); //really not sure why we are copy-constructing this here.
		if (!preExpand) {
			plist mt_parms = std::move(modified.parms);
			modified.parms.clear();
			if (trimParms) { trim(mt_parms); }
			modified.generated = false;
			context.push_back({nullptr,std::move(modified)}); //This is done for injections like %(1+).
			for (auto &parm : mt_parms) {
				mtext expanded;
				auto i=context.back().second.parms.size();
				for(auto& token : parm) {
					token->expand(e,expanded,context);
				}
				if(context.back().second.parms.size() == i) {
					context.back().second.parms.push_back(std::move(expanded)); //each one as a separate parm!!
				}
			}
			modified = std::move(context.back().second);
			context.pop_back();
			modified.generated = instance.generated;
			while (!modified.parms.empty() && modified.parms.back().empty()) {
				modified.parms.pop_back();
			}
			modified.it = {0,modified.parms.size()};
		}
		context.push_front({this,modified});
		if (iterated) {
			Instance& specific = context.front().second;
			iteration* i = &(specific.it); //so we are iterating front (because not all are generated).
			if(specific.myFor != nullptr) {
				for (i->first = 1; i->first <= i->second; i->first++) {
					std::ostringstream value;
					const mtext& parm = specific.parms[i->first - 1];
					Driver::expand(e,parm,value,context);
					mtext done_parm;
					specific.myFor->set(value.str(),i->first); value.str("");
					Driver::doFor(expansion,done_parm,*(specific.myFor));
					for(auto& token : done_parm) {
						token->expand(e,o,context);
					}
//					Driver::expand(e, done_parm, o, context);
				}
			} else {
				for (i->first = 1; i->first <= i->second; i->first++) {
					for(auto& token: expansion) {
						token->expand(e,o,context);
					}
//					Driver::expand(e, expansion, o, context);
				}
			}
		} else {
			for(auto& token: expansion) {
				token->expand(e,o,context);
			}
//			Driver::expand(e, expansion, o, context);
		}
		context.pop_front();
	}

	bool Definition::test_adv(const std::string &basis) {
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

	void Definition::vis(const std::string name,std::ostream& o) {
		auto good = Definition::library.find(name);
		if(good != Definition::library.end()) {
			good->second->visit(o);
		} else {
			o << "«" << name << "»";
		}
	}

	void Definition::exp(const std::string name,Messages& e,mtext& o,Instance& i,mt::mstack& c) {
		auto good = Definition::library.find(name);
		if(good != Definition::library.end()) {
			good->second->expand(e,o,i,c);
		} else {
			e << Message(error,name + " not found.");
		}
	}

	bool Definition::has(const std::string name) {
		return library.find(name) != Definition::library.end();
	}

	void Definition::del(const std::string name) {
		auto good = library.find(name);
		if (good != library.end()) {
			auto* ptr = good->second.release();
			library.erase(good);
			delete ptr;
		}
	}

	void Definition::list(std::ostream& o) {
		for (auto& a :library) {
			o << a.first << ": ";
			vis(a.first,o);
			o << std::endl;
		}
	}

	std::ostream &Definition::visit(std::ostream &o) const {
		return Driver::visit(expansion, o);
	}

	void Definition::trim(plist& bobs) {
		for (auto& j : bobs) { //each parameter.
			while (!j.empty() && (dynamic_cast<Wss *>(j.front().get()) != nullptr) ) {
				j.pop_front();
			}
			while (!j.empty() && (dynamic_cast<Wss *>(j.back().get()) != nullptr) ) {
				j.pop_back();
			}
		}
	}

	void Definition::startup(Messages& log) {
		Timing& times = Timing::t();
		if (times.show()) { times.set("Load Internal Macros"); }

//•------------  Utility macros
		library.emplace("iEq",new iEq());
		library.emplace("iIndex",new iIndex());
		library.emplace("iForIndex",new iForIndex());
		library.emplace("iExpr",new iExpr());
		library.emplace("iConsole",new iConsole());
		library.emplace("iDate",new iDate());
		library.emplace("iEval",new iEval());
		library.emplace("iFile",new iFile());
		library.emplace("iField",new iField());
		library.emplace("iForSubs",new iForSubs());
		library.emplace("iForQuery",new iForQuery());
		library.emplace("iMath",new iMath());
		library.emplace("iNull",new iNull());
		library.emplace("iTiming",new iTiming());

//•------------  Storage macros
		library.emplace("iExists",new iExists());
		library.emplace("iSet",new iSet());
		library.emplace("iGet",new iGet());
		library.emplace("iAppend",new iAppend());
		library.emplace("iKV",new iKV());
		library.emplace("iList",new iList());
		library.emplace("iReset",new iReset());
		library.emplace("iSetCache",new iSetCache());
		library.emplace("iSig",new iSig());
		library.emplace("iUse",new iUse());

//•------------  String macros
		library.emplace("iLeft",new iLeft());
		library.emplace("iLength",new iLength());
		library.emplace("iMid",new iMid());
		library.emplace("iPosition",new iPosition());
		library.emplace("iRegex",new iRegex());
		library.emplace("iRembr",new iRembr());
		library.emplace("iRembrp",new iRembrp());
		library.emplace("iReplace",new iReplace());
		library.emplace("iRight",new iRight());
		library.emplace("iTrim",new iTrim());

//•------------ Encoder macros
		library.emplace("iBase64",new iBase64());
		library.emplace("iDecode",new iDecode());
		library.emplace("iEncode",new iEncode());
		library.emplace("iHex",new iHex());
		library.emplace("iUnHex",new iUnHex());
		library.emplace("iUpper",new iUpper());
		library.emplace("iLower",new iLower());
		library.emplace("iUrlEncode",new iUrlEncode());
		library.emplace("iDigest",new iDigest());

//•------------ NodeTree
		library.emplace("iNumChildren",new iNumChildren());
		library.emplace("iNumGen",new iNumGen());
		library.emplace("iNumGens",new iNumGens());
		library.emplace("iNumPage",new iNumPage());
		library.emplace("iNumPages",new iNumPages());
		library.emplace("iNumSib",new iNumSib());
		library.emplace("iEqFamily",new iEqFamily());
		library.emplace("iEqNode",new iEqNode());
		library.emplace("iEqSibs",new iEqSibs());
		library.emplace("iExistNode",new iExistNode());
		library.emplace("iForAncestry",new iForAncestry());
		library.emplace("iForNodes",new iForNodes());
		library.emplace("iForPeers",new iForPeers());
		library.emplace("iForSibs",new iForSibs());
		library.emplace("iSize",new iSize());

//•------------ BuildAccessors
		library.emplace("iTech",new iTech());
		library.emplace("iPreview",new iPreview());
		library.emplace("iLang",new iLang());
		library.emplace("iLangID",new iLangID());
		library.emplace("iFullBuild",new iFullBuild());

//•------------ NodeAccessors
		library.emplace("iTitle",new iTitle());
		library.emplace("iTeam",new iTeam());
		library.emplace("iSuffix",new iSuffix());
		library.emplace("iShortTitle",new iShortTitle());
		library.emplace("iSegmentName",new iSegmentName());
		library.emplace("iLayout",new iLayout());
		library.emplace("iLayoutName",new iLayoutName());
		library.emplace("iLink",new iLink());
		library.emplace("iLinkRef",new iLinkRef());
		library.emplace("iID",new iID());
		library.emplace("iBirth",new iBirth());
		library.emplace("iContent",new iContent());
		library.emplace("iDeath",new iDeath());
		library.emplace("iExistContent",new iExistContent());
		library.emplace("iTW",new iTW());

//•------------ Media
		library.emplace("iMedia",new iMedia());
		library.emplace("iEmbed",new iEmbed());
		library.emplace("iExistMedia",new iExistMedia());

//•------------ Taxonomy
		library.emplace("iTax",new iTax());
		library.emplace("iForTax",new iForTax());
		library.emplace("iForTaxNodes",new iForTaxNodes());
		library.emplace("iExistSimilar",new iExistSimilar());
		library.emplace("iForSimilar",new iForSimilar());


//•------------ FINISHED
		if (times.show()) { times.use(log,"Load Internal Macros"); }

	}

}
