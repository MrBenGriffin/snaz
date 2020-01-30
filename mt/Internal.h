//
// Created by Ben on 2019-01-24.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H

#include <stack>
#include <climits>
#include "Handler.h"
#include "Definition.h"
#include "mt.h"
#include "support/Storage.h"
#include "support/db/Connection.h"
#include "support/db/Query.h"

namespace node {
	class Node;
	class Metrics;
}

namespace mt {
	class Internal : public Handler {
	private:
		void sortnodes(deque<const node::Node *>&,bool,char) const;

	public:
		bool internal() const override { return true; }
		bool inRange(size_t) const override;
		void doSort(Support::Messages&,deque<const node::Node*>&,std::string,mstack* = nullptr,node::Metrics* = nullptr) const;
		std::string name() const override;
		size_t minParms {0}, maxParms {0};
		static void startup(Messages&,Db::Connection&,buildKind);
		static void shutdown(Messages&,Db::Connection&,buildKind);
		static void reset(string);
		void doTrace(Support::Messages&,mstack&) const;

	protected:
//Not so sure these should be here..
		static Storage storage;
		static Library library;
		static LStore  lStore;    //'list store'  string:multiset(string)
		static Db::Connection* sql;
		static bool just_parsing;

		std::string _name;
		Internal(std::string name,size_t min,size_t max);
		plist toParms(string,string,string) const;
		plist toParms(const listType*,string,size_t = string::npos) const;
		plist toParms(deque<string>&,string,size_t = string::npos) const;
		void doSort(deque<std::string>&,std::string) const;
		void check(Support::Messages&,Instance&,const location&) const override;

	};

	class FieldContext: public Internal {
	protected:
		FieldContext(std::string name,size_t min,size_t max) : Internal(name,min,max) {}

	public:
		virtual string get(Messages&,const string& name) const = 0;
	};

	//•------------ √ Utility macros
	struct iEq : public Internal {
		iEq() : Internal("iEq",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iExpr : public Internal {
		iExpr() : Internal("iExpr",1,INT_MAX) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};

	struct iIndex : public Internal {
		iIndex() : Internal("iIndex",1,6) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};

	struct iForIndex : public Internal {
		iForIndex() : Internal("iForIndex",1,6) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};

	struct iConsole : public Internal {
		iConsole() : Internal("iConsole",1,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iDate : public Internal {
		iDate() : Internal("iDate",0,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iEval : public Internal {
		iEval() : Internal("iEval",1,INT_MAX) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iFile : public Internal {
		iFile() : Internal("iFile",1,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iField : public Internal {
		static std::stack<const FieldContext*> contextStack;
		iField() : Internal("iField",1,1) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForSubs : public FieldContext {
		static Support::Db::Query* query;
		iForSubs() : FieldContext("iForSubs",5,5) {}
		string get(Messages&,const string&) const override;
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForQuery : public FieldContext {
		static Support::Db::Query* query;
		iForQuery() : FieldContext("iForQuery",2,2) {}
		string get(Messages&,const string&) const override;
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iMath : public Internal {
		iMath() : Internal("iMath",2,7) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iNull : public Internal {
		iNull() : Internal("iNull",0,INT_MAX) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iTiming : public Internal {
		iTiming() : Internal("iTiming",0,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};

//•------------ √ Storage macros
	struct iExists : public Internal {
		iExists() : Internal("iExists",1,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iGet : public Internal {
		iGet() : Internal("iGet",1,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iSet : public Internal {
		iSet() : Internal("iSet",1,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iAppend : public Internal {
		iAppend() : Internal("iAppend",1,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iKV : public Internal {
		iKV() : Internal("iKV",2,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iList : public Internal {
		iList() : Internal("iList",2,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iReset : public Internal {
		iReset() : Internal("iReset",1,1) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iSetCache : public Internal {
		iSetCache() : Internal("iSetCache",2,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iSig : public Internal {
		iSig() : Internal("iSig",1,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iUse : public Internal {
		iUse() : Internal("iUse",1,1) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};

//•------------ √ String manipulation macros
	struct iLeft : public Internal {
		iLeft() : Internal("iLeft",2,6) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iLength : public Internal {
		iLength() : Internal("iLength",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iMid : public Internal {
		iMid() : Internal("iMid",3,6) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iPosition : public Internal {
		iPosition() : Internal("iPosition",2,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iRegex : public Internal {
		iRegex() : Internal("iRegex",2,6) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iRembr : public Internal {
		iRembr() : Internal("iRembr",0,1) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iRembrp : public Internal {
		iRembrp() : Internal("iRembrp",0,1) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iReplace : public Internal {
		iReplace() : Internal("iReplace",1,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};

	struct iRight : public Internal {
		iRight() : Internal("iRight",2,6) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iTrim : public Internal {
		iTrim() : Internal("iTrim",0,1) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
//•------------ √ Encoder/Decoder macros
	struct iBase64 : public Internal {
		iBase64() : Internal("iBase64",1,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iDecode : public Internal {
		iDecode() : Internal("iDecode",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iEncode : public Internal {
		iEncode() : Internal("iEncode",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iHex : public Internal {
		iHex() : Internal("iHex",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iUnHex : public Internal {
		iUnHex() : Internal("iUnHex",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iUpper : public Internal {
		iUpper() : Internal("iUpper",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iLower : public Internal {
		iLower() : Internal("iLower",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iUrlEncode : public Internal {
		iUrlEncode() : Internal("iUrlEncode",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iDigest : public Internal {
		iDigest() : Internal("iDigest",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
//•------------ √ NodeTree
	struct iNumChildren : public Internal {
		iNumChildren() : Internal("iNumChildren",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iNumGen : public Internal {
		iNumGen() : Internal("iNumGen",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iNumGens : public Internal {
		iNumGens() : Internal("iNumGens",0,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iNumPage : public Internal {
		iNumPage() : Internal("iNumPage",0,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iNumPages : public Internal {
		iNumPages() : Internal("iNumPages",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iNumSib : public Internal {
		iNumSib() : Internal("iNumSib",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iEqFamily : public Internal {
		iEqFamily() : Internal("iEqFamily",2,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iEqNode : public Internal {
		iEqNode() : Internal("iEqNode",2,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iEqSibs : public Internal {
		iEqSibs() : Internal("iEqSibs",2,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iExistNode : public Internal {
		iExistNode() : Internal("iExistNode",1,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForAncestry : public Internal {
		iForAncestry() : Internal("iForAncestry",5,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForNodes : public Internal {
		iForNodes() : Internal("iForNodes",5,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForPeers : public Internal {
		iForPeers() : Internal("iForPeers",6,6) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForSibs : public Internal {
		iForSibs() : Internal("iForSibs",4,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iSize : public Internal {
		iSize() : Internal("iSize",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
//•------------ BuildAccessors
	struct iTech : public Internal {
		iTech() : Internal("iTech",0,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iPreview : public Internal {
		iPreview() : Internal("iPreview",0,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iLang : public Internal {
		iLang() : Internal("iLang",0,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iLangID : public Internal {
		iLangID() : Internal("iLangID",0,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iFullBuild : public Internal {
		iFullBuild() : Internal("iFullBuild",0,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
//•------------ √ NodeAccessors
	struct iTitle : public Internal {
		iTitle() : Internal("iTitle",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iTeam : public Internal {
		iTeam() : Internal("iTeam",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iSuffix : public Internal {
		iSuffix() : Internal("iSuffix",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iShortTitle : public Internal {
		iShortTitle() : Internal("iShortTitle",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iSegmentName : public Internal {
		iSegmentName() : Internal("iSegmentName",0,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iLayout : public Internal {
		iLayout() : Internal("iLayout",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iLayoutName : public Internal {
		iLayoutName() : Internal("iLayoutName",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iLink : public Internal {
		iLink() : Internal("iLink",0,1) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iLinkRef : public Internal {
		iLinkRef() : Internal("iLinkRef",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iID : public Internal {
		iID() : Internal("iID",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iBirth : public Internal {
		iBirth() : Internal("iBirth",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iContent : public Internal {
		iContent() : Internal("iContent",2,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iDeath : public Internal {
		iDeath() : Internal("iDeath",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iExistContent : public Internal {
		iExistContent() : Internal("iExistContent",2,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iTW : public Internal {
		iTW() : Internal("iTW",0,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
//•------------ Media
	struct iMedia : public Internal {
		iMedia() : Internal("iMedia",1,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iEmbed : public Internal {
		iEmbed() : Internal("iEmbed",1,2) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iExistMedia : public Internal {
		iExistMedia() : Internal("iExistMedia",1,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
//•------------ Taxonomy
	struct iTax : public Internal {
		iTax() : Internal("iTax",2,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForTax : public Internal {
		iForTax() : Internal("iForTax",4,4) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForTaxNodes : public Internal {
		iForTaxNodes() : Internal("iForTaxNodes",5,5) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iExistSimilar : public Internal {
		iExistSimilar() : Internal("iExistSimilar",1,3) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
	struct iForSimilar : public Internal {
		iForSimilar() : Internal("iForSimilar",6,6) {}
		void expand(Support::Messages&,MacroText&,Instance&,mstack&) const override;
	};
//•------------------------

}

#endif //MACROTEXT_INTERNALS_H
