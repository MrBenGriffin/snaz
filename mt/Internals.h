//
// Created by Ben Griffin on 2019-02-07.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H

#include "Definition.h"
#include "support/Storage.h"
#include "mt.h"
#include "Internal.h"
#include "InternalInstance.h"

namespace mt {
	// Utility
	class iEq : public Internal {
		iEq() : Internal("iEq",0,4) {}
		virtual void work(InternalInstance&);
	};
	class iExpr : public Internal {
		iExpr() : Internal("iExpr",1,INT_MAX) {}
		virtual void work(InternalInstance&);
	};

	class iIndex : public Internal {
		iIndex() : Internal("iIndex",1,6) {}
		virtual void work(InternalInstance&);
	};

	class iForIndex : public Internal {
		iForIndex() : Internal("iForIndex",1,6) {}
		virtual void work(InternalInstance&);
	};

	// Storage
	class iExists : public Internal {
		iExists() : Internal("iExists",1,3) {}
		virtual void work(InternalInstance&);
	};
	class iGet : public Internal {
		iGet() : Internal("iGet",1,4) {}
		virtual void work(InternalInstance&);
	};
	class iSet : public Internal {
		iSet() : Internal("iSet",1,2) {}
		virtual void work(InternalInstance&);
	};
	class iAppend : public Internal {
		iAppend() : Internal("iAppend",1,2) {}
		virtual void work(InternalInstance&);
	};
	class iKV : public Internal {
		iKV() : Internal("iKV",2,5) {}
		virtual void work(InternalInstance&);
	};
	class iList : public Internal {
		iList() : Internal("iList",2,5) {}
		virtual void work(InternalInstance&);
	};
	class iReset : public Internal {
		iReset() : Internal("iReset",1,1) {}
		virtual void work(InternalInstance&);
	};
	class iSetCache : public Internal {
		iSetCache() : Internal("iSetCache",2,2) {}
		virtual void work(InternalInstance&);
	};
	class iSig : public Internal {
		iSig() : Internal("iSig",1,3) {}
		virtual void work(InternalInstance&);
	};
	class iUse : public Internal {
		iUse() : Internal("iUse",1,1) {}
		virtual void work(InternalInstance&);
	};

	// Strings
	class iLeft : public Internal {
		iLeft() : Internal("iLeft",2,6) {}
		virtual void work(InternalInstance&);
	};
	class iLength : public Internal {
		iLength() : Internal("iLength",0,4) {}
		virtual void work(InternalInstance&);
	};
	class iMid : public Internal {
		iMid() : Internal("iMid",3,6) {}
		virtual void work(InternalInstance&);
	};
	class iPosition : public Internal {
		iPosition() : Internal("iPosition",2,5) {}
		virtual void work(InternalInstance&);
	};
	class iRegex : public Internal {
		iRegex() : Internal("iRegex",2,6) {}
		virtual void work(InternalInstance&);
	};
	class iRembr : public Internal {
		iRembr() : Internal("iRembr",0,1) {}
		virtual void work(InternalInstance&);
	};
	class iRembrp : public Internal {
		iRembrp() : Internal("iRembrp",0,1) {}
		virtual void work(InternalInstance&);
	};
	class iReplace : public Internal {
		iReplace() : Internal("iReplace",1,5) {}
		virtual void work(InternalInstance&);
	};

	class iRight : public Internal {
		iRight() : Internal("iRight",2,6) {}
		virtual void work(InternalInstance&);
	};
	class iTrim : public Internal {
		iTrim() : Internal("iTrim",0,1) {}
		virtual void work(InternalInstance&);
	};

};


#endif //MACROTEXT_INTERNALS_H
