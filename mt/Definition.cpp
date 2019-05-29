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

	std::unordered_map<std::string, std::unique_ptr<Handler> > Definition::library;

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
				size_t pre,stripDef,stripParms;
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
						auto macro = make_unique<Definition>(log,name,expansion,min,max,stripDef==1,stripParms==1,pre==1);
//						Definition* macro = new Definition(log,name,expansion,min,max,stripDef==1,stripParms==1,pre==1);
						if(log.marked()) {
							str.str(""); str << "Define `" << name << "` failed while parsing `" << expansion << "`";
							errs << Message(error,str.str());
							errs += log;
							macro.reset();
						} else {
							del(name);
							library.emplace(name,move(macro));
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
		library.clear();  //May need to do this (if there are internal dependencies) because it's a static.
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

	void Definition::expand(Support::Messages& e,mtext& o,shared_ptr<Instance>& instance, mstack &context) const {
		if(!parmCheck(e,instance->size())) {
			return;
		}
		if (!preExpand && ! instance->parms.empty()) {
			//We are evaluating the parms within the context of the calling macro.
			plist passedParms = std::move(instance->parms);
			instance->parms.clear();
			if (trimParms) { trim(passedParms); }
			instance->generated = false;
			//We push this back in order to pass lists to it...
			context.emplace_back(make_pair(nullptr,instance)); //This is done for injections like %(1+).
			for (auto &parm : passedParms) {
				mtext expanded;
				auto i= instance->size();
				for(auto& token : parm) {
					token->expand(e,expanded,context);
				}
				if(instance->size() == i) {
					instance->parms.emplace_back(std::move(expanded)); //each one as a separate parm!!
				}
			}
			context.pop_back();
			instance->tidy();
		}
		context.emplace_front(make_pair(this,instance));
		if (iterated) {
			if(instance->myFor == nullptr) {
				auto& i = context.front().second->it;
				while ( i.first <= i.second) {
					for(auto& token: expansion) {
						token->expand(e,o,context);
					}
					i.first++;
				}
			} else {
				assert(false);
				/**
				 * doFor is now done elsewhere, and this is not needed, I think!!
				Instance specific(instance);
				iteration* i = &(specific.it); //so we are iterating front (because not all are generated).
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
				}
				 */
			}
		} else {
			for(auto& token: expansion) {
				token->expand(e,o,context);
			}
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

	void Definition::vis(const std::string& name,std::ostream& o) {
		auto good = Definition::library.find(name);
		if(good != Definition::library.end()) {
			good->second->visit(o);
		} else {
			o << "«" << name << "»";
		}
	}
	void Definition::exp(const std::string name,Messages& e,mtext& o,shared_ptr<Instance>& i,mt::mstack& c) {
		auto good = Definition::library.find(name);
		if(good != Definition::library.end()) {
			good->second->expand(e,o,i,c);
		} else {
			e << Message(error,name + " not found.");
		}
	}

	bool Definition::has(const std::string& name) {
		return library.find(name) != Definition::library.end();
	}

	void Definition::del(const std::string& name) {
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
		library.emplace("iEq",make_unique<iEq>());
		library.emplace("iIndex",make_unique<iIndex>());
		library.emplace("iForIndex",make_unique<iForIndex>());
		library.emplace("iExpr",make_unique<iExpr>());
		library.emplace("iConsole",make_unique<iConsole>());
		library.emplace("iDate",make_unique<iDate>());
		library.emplace("iEval",make_unique<iEval>());
		library.emplace("iFile",make_unique<iFile>());
		library.emplace("iField",make_unique<iField>());
		library.emplace("iForSubs",make_unique<iForSubs>());
		library.emplace("iForQuery",make_unique<iForQuery>());
		library.emplace("iMath",make_unique<iMath>());
		library.emplace("iNull",make_unique<iNull>());
		library.emplace("iTiming",make_unique<iTiming>());

//•------------  Storage macros
		library.emplace("iExists",make_unique<iExists>());
		library.emplace("iSet",make_unique<iSet>());
		library.emplace("iGet",make_unique<iGet>());
		library.emplace("iAppend",make_unique<iAppend>());
		library.emplace("iKV",make_unique<iKV>());
		library.emplace("iList",make_unique<iList>());
		library.emplace("iReset",make_unique<iReset>());
		library.emplace("iSetCache",make_unique<iSetCache>());
		library.emplace("iSig",make_unique<iSig>());
		library.emplace("iUse",make_unique<iUse>());

//•------------  String macros
		library.emplace("iLeft",make_unique<iLeft>());
		library.emplace("iLength",make_unique<iLength>());
		library.emplace("iMid",make_unique<iMid>());
		library.emplace("iPosition",make_unique<iPosition>());
		library.emplace("iRegex",make_unique<iRegex>());
		library.emplace("iRembr",make_unique<iRembr>());
		library.emplace("iRembrp",make_unique<iRembrp>());
		library.emplace("iReplace",make_unique<iReplace>());
		library.emplace("iRight",make_unique<iRight>());
		library.emplace("iTrim",make_unique<iTrim>());

//•------------ Encoder macros
		library.emplace("iBase64",make_unique<iBase64>());
		library.emplace("iDecode",make_unique<iDecode>());
		library.emplace("iEncode",make_unique<iEncode>());
		library.emplace("iHex",make_unique<iHex>());
		library.emplace("iUnHex",make_unique<iUnHex>());
		library.emplace("iUpper",make_unique<iUpper>());
		library.emplace("iLower",make_unique<iLower>());
		library.emplace("iUrlEncode",make_unique<iUrlEncode>());
		library.emplace("iDigest",make_unique<iDigest>());

//•------------ NodeTree
		library.emplace("iNumChildren",make_unique<iNumChildren>());
		library.emplace("iNumGen",make_unique<iNumGen>());
		library.emplace("iNumGens",make_unique<iNumGens>());
		library.emplace("iNumPage",make_unique<iNumPage>());
		library.emplace("iNumPages",make_unique<iNumPages>());
		library.emplace("iNumSib",make_unique<iNumSib>());
		library.emplace("iEqFamily",make_unique<iEqFamily>());
		library.emplace("iEqNode",make_unique<iEqNode>());
		library.emplace("iEqSibs",make_unique<iEqSibs>());
		library.emplace("iExistNode",make_unique<iExistNode>());
		library.emplace("iForAncestry",make_unique<iForAncestry>());
		library.emplace("iForNodes",make_unique<iForNodes>());
		library.emplace("iForPeers",make_unique<iForPeers>());
		library.emplace("iForSibs",make_unique<iForSibs>());
		library.emplace("iSize",make_unique<iSize>());

//•------------ BuildAccessors
		library.emplace("iTech",make_unique<iTech>());
		library.emplace("iPreview",make_unique<iPreview>());
		library.emplace("iLang",make_unique<iLang>());
		library.emplace("iLangID",make_unique<iLangID>());
		library.emplace("iFullBuild",make_unique<iFullBuild>());

//•------------ NodeAccessors
		library.emplace("iTitle",make_unique<iTitle>());
		library.emplace("iTeam",make_unique<iTeam>());
		library.emplace("iSuffix",make_unique<iSuffix>());
		library.emplace("iShortTitle",make_unique<iShortTitle>());
		library.emplace("iSegmentName",make_unique<iSegmentName>());
		library.emplace("iLayout",make_unique<iLayout>());
		library.emplace("iLayoutName",make_unique<iLayoutName>());
		library.emplace("iLink",make_unique<iLink>());
		library.emplace("iLinkRef",make_unique<iLinkRef>());
		library.emplace("iID",make_unique<iID>());
		library.emplace("iBirth",make_unique<iBirth>());
		library.emplace("iContent",make_unique<iContent>());
		library.emplace("iDeath",make_unique<iDeath>());
		library.emplace("iExistContent",make_unique<iExistContent>());
		library.emplace("iTW",make_unique<iTW>());

//•------------ Media
		library.emplace("iMedia",make_unique<iMedia>());
		library.emplace("iEmbed",make_unique<iEmbed>());
		library.emplace("iExistMedia",make_unique<iExistMedia>());

//•------------ Taxonomy
		library.emplace("iTax",make_unique<iTax>());
		library.emplace("iForTax",make_unique<iForTax>());
		library.emplace("iForTaxNodes",make_unique<iForTaxNodes>());
		library.emplace("iExistSimilar",make_unique<iExistSimilar>());
		library.emplace("iForSimilar",make_unique<iForSimilar>());

//•------------ FINISHED
		if (times.show()) { times.use(log,"Load Internal Macros"); }

	}

}
