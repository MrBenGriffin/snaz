//
// Created by Ben on 2019-02-28.
//

#ifndef MACROTEXT_BUILD_H
#define MACROTEXT_BUILD_H

#include <unordered_map>
#include <set>

#include "Node.h"
#include "support/Message.h"
#include "support/db/Connection.h"

using namespace std;
using namespace Support;
using namespace Support::Db;

using refmaptype = unordered_map<string, size_t>;
using idmaptype = unordered_map<size_t, Node* >;
using rfmaptype = unordered_map<string, Node* >;
using intintdequemap = unordered_map<size_t, deque<size_t> >;
using templatemap = unordered_map<size_t, pair< string, FileTemplate >> ;
using uintstringmaptype = unordered_map<size_t, string>;	// row position in query -> filename
using layoutsegment_type = set< pair<size_t,size_t>>;
using layoutsegref_type =  unordered_map<pair<size_t,string>, size_t>;
using reftointmap = unordered_map<string, size_t >;

//typedef pair<size_t,string> pairtype;
//typedef unordered_map <size_t, set< nodeaccessor > > nnadepsmap_type; //node to node-accessor
//typedef unordered_map <size_t, set< nodesegment > > nnsdepsmap_type;  //node to node-segment
//typedef map<pairtype, size_t> layoutsegref_type;
//typedef map<pairtype, vector<string> > content_map_type;
//typedef unordered_map<size_t, vector<size_t> > intintvectmap;
//typedef unordered_map<string, vector<size_t> > strintvectmap;
//typedef unordered_map<string, vector<string> > strvecmap;
//typedef unordered_map<size_t, pair< string , filetemplate >  > templatemap;
//typedef unordered_map<size_t, Node* > idmaptype;
//typedef unordered_map<string, Node* > rfmaptype;
//typedef unordered_map<string, size_t> refmaptype;	// generic string to integer map.
////typedef unordered_map<string, string> storage_map_type;
//typedef unordered_map<string, string> dirset;
//typedef unordered_map<size_t, vector<string> > vecmaptype;
//typedef unordered_map<string, pair<size_t,string> > segmentidtype;
////typedef unordered_map<string, storage_map_type* > strmap_map_type;
//typedef multimap<int, vector<string> *> dbmaptype;
//typedef multimap<pair<size_t,size_t>,size_t> taxmaptype;
//typedef multimap< size_t, pair<double,size_t> > similarmaptype;
//typedef multimap<size_t,size_t> uintuintmaptype;

struct FileTemplate {
	string str;
	string suffix;
	string br;
};

class Build {

	Db::Connection*	mysql;
	templatemap			templateList;
	intintdequemap		layoutList;				//Stores a layout-id -> template-list structure
	uintstringmaptype	segmentNames;			//A quick reference to the segment names by ID..
	layoutsegment_type	layoutSegments;			//Stores a [layout,segment] structure.
	layoutsegref_type	layoutSegRefs;
	reftointmap			layoutRefs;				//Layout names -> ids
	uintstringmaptype	layoutNames;			//Layout ids->names

	deque< Node* > 	node_stack;				//current node - used to pass to built-in functions

	Node* roott;
	Node* rootc;
	Node* roots;
	size_t layoutCount;

	refmaptype tax_fields;								//A quick reference to the fields by name (as in iTax)

	Build(); // Disallow instantiation outside of the class.

//Setup
	void inittaxfields();
	void init(Messages&,Connection& dbc);
	void loadLayouts(Messages&);

public:
	static Build& b();
	Node* current(Node::kind = Node::page);
	Node* node(size_t,Node::kind = Node::page); //by id.


};


#endif //MACROTEXT_BUILD_H
