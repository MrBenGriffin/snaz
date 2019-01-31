//
// Created by Ben on 2019-01-24.
//

#ifndef MACROTEXT_INTERNALS_H
#define MACROTEXT_INTERNALS_H


#include "Definition.h"
#include "mt.h"

namespace mt {
	struct Internal {
		std::string _name;
		size_t minParms {0}, maxParms {0};
		bool inRange(size_t i) const { return (minParms <= i) && (i <= maxParms);}
		std::string name() const {return _name;}
		Internal(std::string name,size_t min,size_t max) : _name(name),minParms(min),maxParms(max) {}
//		virtual void expand(mtext&,Instance&,mstack&) = 0;
	};
	struct iEq : public Internal {
		iEq() : Internal("iEq",0,4) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};
	struct iExpr : public Internal {
		iExpr() : Internal("iExpr",1,INT_MAX) {}
		void expand(Support::Messages&,mtext&,Instance&,mstack&);
	};

}


#endif //MACROTEXT_INTERNALS_H
