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
	struct iEq;
	struct iIndex;
	struct iForIndex;
	struct iExpr;
	struct iConsole;
	struct iDate;
	struct iEval;
	struct iFile;
	struct iField;
	struct iForSubs;
	struct iForQuery;
	struct iMath;
	struct iNull;
	struct iTiming;

//•------------  Storage macros
	struct iExists;
	struct iSet;
	struct iGet;
	struct iAppend;
	struct iKV;
	struct iList;
	struct iReset;
	struct iSetCache;
	struct iSig;
	struct iUse;

//•------------  String macros
	struct iLeft;
	struct iLength;
	struct iMid;
	struct iPosition;
	struct iRegex;
	struct iRembr;
	struct iRembrp;
	struct iReplace;
	struct iRight;
	struct iTrim;

//•------------ Encoder macros
	struct iBase64;
	struct iDecode;
	struct iEncode;
	struct iHex;
	struct iUnHex;
	struct iUpper;
	struct iLower;
	struct iUrlEncode;
	struct iDigest;

//•------------ NodeTree
	struct iNumChildren;
	struct iNumGen;
	struct iNumGens;
	struct iNumPage;
	struct iNumPages;
	struct iNumSib;
	struct iEqFamily;
	struct iEqNode;
	struct iEqSibs;
	struct iExistNode;
	struct iForAncestry;
	struct iForNodes;
	struct iForPeers;
	struct iForSibs;
	struct iSize;

//•------------ BuildAccessors
	struct iTech;
	struct iPreview;
	struct iLang;
	struct iLangID;
	struct iFullBuild;

//•------------ NodeAccessors
	struct iTitle;
	struct iTeam;
	struct iSuffix;
	struct iShortTitle;
	struct iSegmentName;
	struct iLayout;
	struct iLayoutName;
	struct iLink;
	struct iLinkRef;
	struct iID;
	struct iBirth;
	struct iContent;
	struct iDeath;
	struct iExistContent;
	struct iTW;

//•------------ Media
	struct iMedia;
	struct iEmbed;
	struct iExistMedia;

//•------------ Taxonomy
	struct iTax;
	struct iForTax;
	struct iForTaxNodes;
	struct iExistSimilar;
	struct iForSimilar;

}

#endif //MACROTEXT_DECLARATIONS_H
