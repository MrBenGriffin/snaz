//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_DECLARATIONS_H
#define MACROTEXT_DECLARATIONS_H

#include <deque>

namespace mt {

	//The following are Token classes.
	class Token;

	//The following are Handler classes.
	//Remember to add these to  Definition::initialise() on line 180 of Definition.cpp
	class Definition;
	class Content;

//•------------  Utility macros
	class iEq;
	class iIndex;
	class iForIndex;
	class iExpr;
	class iConsole;
	class iDate;
	class iEval;
	class iFile;
	class iField;
	class iForSubs;
	class iForQuery;
	class iMath;
	class iNull;
	class iTiming;

//•------------  Storage macros
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

//•------------  String macros
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

//•------------ Encoder macros
	class iBase64;
	class iDecode;
	class iEncode;
	class iHex;
	class iUnHex;
	class iUpper;
	class iLower;
	class iUrlEncode;
	class iDigest;

//•------------ NodeTree
	class iNumChildren;
	class iNumGen;
	class iNumGens;
	class iNumPage;
	class iNumPages;
	class iNumSib;
	class iEqFamily;
	class iEqNode;
	class iEqSibs;
	class iExistNode;
	class iForAncestry;
	class iForNodes;
	class iForPeers;
	class iForSibs;
	class iSize;

//•------------ BuildAccessors
	class iTech;
	class iPreview;
	class iLang;
	class iLangID;
	class iFullBuild;

//•------------ NodeAccessors
	class iTitle;
	class iTeam;
	class iSuffix;
	class iShortTitle;
	class iSegmentName;
	class iLayout;
	class iLayoutName;
	class iLink;
	class iLinkRef;
	class iID;
	class iBirth;
	class iContent;
	class iDeath;
	class iExistContent;
	class iTW;

//•------------ Media
	class iMedia;
	class iEmbed;
	class iExistMedia;

//•------------ Taxonomy
	class iTax;
	class iForTax;
	class iForTaxNodes;
	class iExistSimilar;
	class iForSimilar;

}

#endif //MACROTEXT_DECLARATIONS_H
