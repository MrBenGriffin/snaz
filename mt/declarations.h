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

//•------------ Encoder/Decoder macros
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

	using Handler=std::variant<Content,Definition,
			//•------------ Utility
			iEq,iExpr,iIndex,iForIndex,iConsole,iDate,iEval,iFile,iField,iForSubs,iForQuery,iMath,iNull,iTiming,
			//•------------ Storage
			iSet,iGet,iExists,iAppend,iKV,iList,iReset,iSetCache,iSig,iUse,
			//•------------ String
			iLeft,iLength,iMid,iPosition,iRegex,iRembr,iRembrp,iReplace,iRight,iTrim,
			//•------------ Encoder
			iBase64,iDecode,iEncode,iHex,iUnHex,iUpper,iLower,iUrlEncode,iDigest,
			//•------------ NodeTree
			iNumChildren,iNumGen,iNumGens,iNumPage,iNumPages,iNumSib,iEqFamily,iEqNode,iEqSibs,iExistNode,iForAncestry,iForNodes,iForPeers,iForSibs,iSize,
			//•------------ Build
			iTech,iPreview,iLang,iLangID,iFullBuild,
			//•------------ Node
			iTitle,iTeam,iSuffix,iShortTitle,iSegmentName,iLayout,iLayoutName,iLink,iLinkRef,iID,iBirth,iContent,iDeath,iExistContent,iTW,
			//•------------ Media
			iMedia,iEmbed,iExistMedia,
			//•------------ Taxonomy
			iTax,iForTax,iForTaxNodes,iExistSimilar,iForSimilar
	>;

}

#endif //MACROTEXT_DECLARATIONS_H
