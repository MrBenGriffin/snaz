//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_DECLARATIONS_H
#define MACROTEXT_DECLARATIONS_H

namespace mt {

	//The following are Token classes.
	class Macro;
	class Injection;
	class Wss;
	class Text;
	using Token=std::variant<Macro,Wss,Injection,Text>;

	//The following are Handler classes.
	//Remember to add these to  Definition::initialise() on line 180 of Definition.cpp
	class Definition;
	class Content;

	//utility
	class iEq;
	class iIndex;
	class iForIndex;
	class iExpr;

	//storage
	class iExists;
	class iSet;
	class iGet;
	class iAppend;
	class iKV;
	class iList;
	class iReset;
	class iSetCache;
	class iSig;
	class iUse;

	//string
	class iLeft;
	class iLength;
	class iMid;
	class iPosition;
	class iRegex;
	class iRembr;
	class iRembrp;
	class iReplace;
	class iRight;
	class iTrim;

	using Handler=std::variant<Content,Definition,
	iEq,iExpr,iIndex,iForIndex,
	iSet,iGet,iExists,iAppend,iKV,iList,iReset,iSetCache,iSig,iUse,
	iLeft,iLength,iMid,iPosition,iRegex,iRembr,iRembrp,iReplace,iRight,iTrim
	>;

}

#endif //MACROTEXT_DECLARATIONS_H
