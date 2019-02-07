//
// Created by Ben on 2019-01-24.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H


#include "Definition.h"
#include "support/Storage.h"
#include "mt.h"

namespace mt {

	class Internal {
	public:
		bool inRange(size_t i) const { return (minParms <= i) && (i <= maxParms);}
		std::string name() const {return _name;}
		size_t minParms {0}, maxParms {0};
	protected:
		static Storage storage;
		static Library library;
		static Library cache;
		static LStore lStore;
		std::string _name;
		Internal(std::string name,size_t min,size_t max) : _name(name),minParms(min),maxParms(max) {}
		plist toParms(string,string,string);
		plist toParms(const listType*,string);
		void doSort(vector<std::string>&,std::string);
	};
	// Utility
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

	// Storage
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

	// Strings
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

}

#endif //MACROTEXT_INTERNALS_H
