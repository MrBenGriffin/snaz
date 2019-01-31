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

	//string
	class iLeft;
	class iMid;
	class iRight;

	using Handler=std::variant<Content,Definition,iEq,iExpr,iIndex,iForIndex,iSet,iGet,iExists,iLeft,iMid,iRight>;

}

#endif //MACROTEXT_DECLARATIONS_H
