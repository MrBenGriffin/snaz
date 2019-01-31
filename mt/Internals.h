//
// Created by Ben on 2019-01-24.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H


#include "Definition.h"
#include "support/Storage.h"
#include "mt.h"

namespace mt {
	struct Internal {
		static Storage storage;
		static Library library;
		static Library cache;
		static LStore lStore;

		std::string _name;
		size_t minParms {0}, maxParms {0};
		bool inRange(size_t i) const { return (minParms <= i) && (i <= maxParms);}
		std::string name() const {return _name;}
		Internal(std::string name,size_t min,size_t max) : _name(name),minParms(min),maxParms(max) {}
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

	// Strings
	struct iLeft : public Internal {
		iLeft() : Internal("iLeft",2,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iMid : public Internal {
		iMid() : Internal("iMid",3,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iRight : public Internal {
		iRight() : Internal("iRight",2,6) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};

}

#endif //MACROTEXT_INTERNALS_H
