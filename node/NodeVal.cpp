//
// Created by Ben on 2019-02-27.
//

#include "NodeVal.h"

string NodeVal::empty="";

// -------------------------------------------------------------------
size_t NodeVal::getival(size_t i) {
	if (i < ivals.size() )
		return ivals[i];
	else
		return 0;
}

// -------------------------------------------------------------------
string& NodeVal::getsval(size_t i) {
	if (i < svals.size() )
		return svals[i];
	else
		return empty;
}
// -------------------------------------------------------------------
void NodeVal::setival(size_t i,size_t v) {
	if (i < ivals.size() )
		ivals[i] = v;
}
// -------------------------------------------------------------------
void NodeVal::setsval(size_t i,string& s) {
	if (i < svals.size() )
		svals[i] = s;
}
// -------------------------------------------------------------------
string& NodeVal::getfilename(size_t i) {
	if ( i >= filename.size() ) return empty;
	return filename[i];
}
// -------------------------------------------------------------------
string& NodeVal::getsuffix(size_t i) {
	if ( i >= suffixes.size() ) return empty;
	return suffixes[i];
}
// -------------------------------------------------------------------
string& NodeVal::getffilename(size_t i) {
	size_t j = ffilename.size();
	if ( i >= j ) {
		return empty;
	} else {
		return ffilename[i];
	}
}
// -------------------------------------------------------------------
size_t NodeVal::gettemplate(size_t i) {
	if ( i >= tplates.size() ) return 0;
	return tplates[i];
}
// -------------------------------------------------------------------
NodeVal::NodeVal() { // constructor used for root only.
	idm = nullptr;
	twm = nullptr;
	rfm = nullptr;
	tierdepth = nullptr;
	svals.clear();
	ivals.clear();
	filename.clear();			//This is the initial page filename (includes . )
	ffilename.clear();			//This is the full, final filename (post-processed)
	suffixes.clear();			//This is the initial page suffix.
	tplates.clear();			// list of template ID's to use
}

// -------------------------------------------------------------------
void NodeVal::reset() {
	filename.clear();			//This is the initial page filename (includes . )
	ffilename.clear();		//This is the full, final filename (post-processed)
	suffixes.clear();			//This is the initial page suffix.
	tplates.clear();			//List of template ID's to use
	setival(2,0);
}
// -------------------------------------------------------------------
NodeVal::~NodeVal() {			// clear the lot of it. hey why not.
	idm = nullptr;
	twm = nullptr;
	rfm = nullptr;
	tierdepth = nullptr;
	svals.clear();
	ivals.clear();
	filename.clear();			//This is the initial page filename (includes . )
	ffilename.clear();		//This is the full, final filename (post-processed)
	suffixes.clear();			//This is the initial page suffix.
	tplates.clear();			//List of template ID's to use
}

// -------------------------------------------------------------------
NodeVal::NodeVal( idmaptype* i, idmaptype* t, rfmaptype* r, size_t* d) {  //ref. constructor
	idm = i;
	twm = t;
	rfm = r;
	tierdepth = d;
	svals.clear();
	ivals.clear();
	filename.clear();			//This is the initial page filename (includes . )
	ffilename.clear();			//This is the full, final filename (post-processed)
	suffixes.clear();			//This is the initial page suffix.
	tplates.clear();			// list of template ID's to use

}
// -------------------------------------------------------------------
