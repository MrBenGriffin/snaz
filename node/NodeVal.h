//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_NODEVAL_H
#define MACROTEXT_NODEVAL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

class Node;

using idmaptype=unordered_map< size_t, Node* > ;
using rfmaptype=unordered_map< string, Node* >;

class NodeVal {	//base class for Content,Tax,Suffixes
protected:
	static string empty;
//references to leaf-class statics - cannot be static here, because there are 3 flavours.
//	NodeLocator* locator;		// node locator of this leaf-class
	idmaptype* idm;				// map nodes by id
	idmaptype* twm;				// map nodes by tw
	rfmaptype* rfm;				// map nodes by linkref
	size_t*  tierdepth;	// tierdepth of this leaf-class

public:
	vector<string>			svals;
	vector<size_t>	ivals;
	string					scratchpad;	//used for iForPeers <- maybe should go elsewhere..

//All of these are used only by content. Never mind. For the moment.
	vector<string> filename;			//This is the initial page filename (includes . )
	vector<string> suffixes;			//This is the initial page suffix.
	vector<string> ffilename;			//This is the full, final filename (post-processed)
	vector<size_t> tplates;		// list of template ID's to use

//functions
//	void setenv(Connection*,size_t,ostream *,string,bool);

	string& getfilename(size_t);
	string& getsuffix(size_t);
	string& getffilename(size_t);
	size_t gettemplate(size_t);

	size_t getival(size_t i);
	string& getsval(size_t i);
	void setival(size_t i,size_t v);
	void setsval(size_t i,string& s);

	NodeVal();
	NodeVal( idmaptype*, idmaptype*, rfmaptype*, size_t* );  //constructor
	virtual ~NodeVal();		//deleting a Node deletes all of it's children
	virtual void reset();

	friend class Node;		//For the time being, we want Node to be able to access idm, twm, etc.
};


#endif //MACROTEXT_NODEVAL_H
