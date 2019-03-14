//
// Created by Ben on 2019-01-24.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H

#include <climits>
#include "Definition.h"
#include "mt.h"
#include "support/Storage.h"
#include "support/db/Connection.h"

namespace mt {
	class Internal {
	public:
		bool inRange(size_t i) const { return (minParms <= i) && (i <= maxParms);}
		std::string name() const {return _name;}
		size_t minParms {0}, maxParms {0};
		static void startup(Messages&,Db::Connection&,buildKind);
		static void shutdown(Messages&,Db::Connection&,buildKind);

	protected:
//Not so sure these should be here..
		static Storage storage;
		static Library library;
		static LStore  lStore;    //'list store'  string:multiset(string)

		std::string _name;
		Internal(std::string name,size_t min,size_t max) : _name(std::move(name)),minParms(min),maxParms(max) {}
		plist toParms(string,string,string);
		plist toParms(const listType*,string);
		void doSort(vector<std::string>&,std::string);
		void doTrace(Support::Messages&,mstack&);

	};
//•------------ √ Utility macros
	struct iEq : public Internal {
		iEq() : Internal("iEq",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iExpr : public Internal {
		iExpr() : Internal("iExpr",1,INT_MAX) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};

	struct iIndex : public Internal {
		iIndex() : Internal("iIndex",1,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};

	struct iForIndex : public Internal {
		iForIndex() : Internal("iForIndex",1,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};

	struct iConsole : public Internal {
		iConsole() : Internal("iConsole",1,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iDate : public Internal {
		iDate() : Internal("iDate",0,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iEval : public Internal {
		iEval() : Internal("iEval",1,INT_MAX) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iFile : public Internal {
		iFile() : Internal("iFile",1,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iField : public Internal {
		iField() : Internal("iField",1,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForSubs : public Internal {
		iForSubs() : Internal("iForSubs",5,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForQuery : public Internal {
		iForQuery() : Internal("iForQuery",2,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iMath : public Internal {
		iMath() : Internal("iMath",2,7) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iNull : public Internal {
		iNull() : Internal("iNull",0,INT_MAX) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iTiming : public Internal {
		iTiming() : Internal("iTiming",0,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};

//•------------ √ Storage macros
	struct iExists : public Internal {
		iExists() : Internal("iExists",1,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iGet : public Internal {
		iGet() : Internal("iGet",1,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iSet : public Internal {
		iSet() : Internal("iSet",1,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iAppend : public Internal {
		iAppend() : Internal("iAppend",1,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iKV : public Internal {
		iKV() : Internal("iKV",2,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iList : public Internal {
		iList() : Internal("iList",2,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iReset : public Internal {
		iReset() : Internal("iReset",1,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iSetCache : public Internal {
		iSetCache() : Internal("iSetCache",2,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iSig : public Internal {
		iSig() : Internal("iSig",1,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iUse : public Internal {
		iUse() : Internal("iUse",1,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};

//•------------ √ String manipulation macros
	struct iLeft : public Internal {
		iLeft() : Internal("iLeft",2,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iLength : public Internal {
		iLength() : Internal("iLength",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iMid : public Internal {
		iMid() : Internal("iMid",3,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iPosition : public Internal {
		iPosition() : Internal("iPosition",2,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iRegex : public Internal {
		iRegex() : Internal("iRegex",2,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iRembr : public Internal {
		iRembr() : Internal("iRembr",0,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iRembrp : public Internal {
		iRembrp() : Internal("iRembrp",0,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iReplace : public Internal {
		iReplace() : Internal("iReplace",1,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};

	struct iRight : public Internal {
		iRight() : Internal("iRight",2,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iTrim : public Internal {
		iTrim() : Internal("iTrim",0,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
//•------------ √ Encoder/Decoder macros
	struct iBase64 : public Internal {
		iBase64() : Internal("iBase64",1,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iDecode : public Internal {
		iDecode() : Internal("iDecode",0,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iEncode : public Internal {
		iEncode() : Internal("iEncode",0,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iHex : public Internal {
		iHex() : Internal("iHex",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iUnHex : public Internal {
		iUnHex() : Internal("iUnHex",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iUpper : public Internal {
		iUpper() : Internal("iUpper",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iLower : public Internal {
		iLower() : Internal("iLower",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iUrlEncode : public Internal {
		iUrlEncode() : Internal("iUrlEncode",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iDigest : public Internal {
		iDigest() : Internal("iDigest",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
//•------------ √ NodeTree
	struct iNumChildren : public Internal {
		iNumChildren() : Internal("iNumChildren",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iNumGen : public Internal {
		iNumGen() : Internal("iNumGen",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iNumGens : public Internal {
		iNumGens() : Internal("iNumGens",0,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iNumPage : public Internal {
		iNumPage() : Internal("iNumPage",0,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iNumPages : public Internal {
		iNumPages() : Internal("iNumPages",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iNumSib : public Internal {
		iNumSib() : Internal("iNumSib",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iEqFamily : public Internal {
		iEqFamily() : Internal("iEqFamily",2,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iEqNode : public Internal {
		iEqNode() : Internal("iEqNode",2,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iEqSibs : public Internal {
		iEqSibs() : Internal("iEqSibs",2,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iExistNode : public Internal {
		iExistNode() : Internal("iExistNode",1,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForAncestry : public Internal {
		iForAncestry() : Internal("iForAncestry",5,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForNodes : public Internal {
		iForNodes() : Internal("iForNodes",5,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForPeers : public Internal {
		iForPeers() : Internal("iForPeers",6,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForSibs : public Internal {
		iForSibs() : Internal("iForSibs",4,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iSize : public Internal {
		iSize() : Internal("iSize",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
//•------------ BuildAccessors
	struct iTech : public Internal {
		iTech() : Internal("iTech",0,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iPreview : public Internal {
		iPreview() : Internal("iPreview",0,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iLang : public Internal {
		iLang() : Internal("iLang",0,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iLangID : public Internal {
		iLangID() : Internal("iLangID",0,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iFullBuild : public Internal {
		iFullBuild() : Internal("iFullBuild",0,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
//•------------ √ NodeAccessors
	struct iTitle : public Internal {
		iTitle() : Internal("iTitle",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iTeam : public Internal {
		iTeam() : Internal("iTeam",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iSuffix : public Internal {
		iSuffix() : Internal("iSuffix",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iShortTitle : public Internal {
		iShortTitle() : Internal("iShortTitle",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iSegmentName : public Internal {
		iSegmentName() : Internal("iSegmentName",0,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iLayout : public Internal {
		iLayout() : Internal("iLayout",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iLayoutName : public Internal {
		iLayoutName() : Internal("iLayoutName",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iLink : public Internal {
		iLink() : Internal("iLink",0,1) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iLinkRef : public Internal {
		iLinkRef() : Internal("iLinkRef",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iID : public Internal {
		iID() : Internal("iID",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iBirth : public Internal {
		iBirth() : Internal("iBirth",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iContent : public Internal {
		iContent() : Internal("iContent",2,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iDeath : public Internal {
		iDeath() : Internal("iDeath",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iExistContent : public Internal {
		iExistContent() : Internal("iExistContent",2,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iTW : public Internal {
		iTW() : Internal("iTW",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
//•------------ Media
	struct iMedia : public Internal {
		iMedia() : Internal("iMedia",1,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iEmbed : public Internal {
		iEmbed() : Internal("iEmbed",1,2) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iExistMedia : public Internal {
		iExistMedia() : Internal("iExistMedia",1,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
//•------------ Taxonomy
	struct iTax : public Internal {
		iTax() : Internal("iTax",2,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForTax : public Internal {
		iForTax() : Internal("iForTax",4,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForTaxNodes : public Internal {
		iForTaxNodes() : Internal("iForTaxNodes",5,5) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iExistSimilar : public Internal {
		iExistSimilar() : Internal("iExistSimilar",1,3) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iForSimilar : public Internal {
		iForSimilar() : Internal("iForSimilar",6,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
//•------------------------

}

#endif //MACROTEXT_INTERNALS_H
