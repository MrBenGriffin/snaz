#ifndef node_h
#define node_h

#include <string>
#include <utility>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>

//#include "nodeval.h"
#include "NodeFilename.h"
#include "support/Message.h"
#include "support/db/Connection.h"

#define kRootID				0				/* id of gRoot */

using namespace std;
using namespace Support;
using namespace Support::Db;

class NodeLocator;
class Node;
class NodeVal;
class FileTemplate;

using refmaptype = unordered_map<string, size_t>;
using idmaptype = unordered_map<size_t, Node* >;
using rfmaptype = unordered_map<string, Node* >;
using intintvectmap = unordered_map<size_t, vector<size_t> >;
using templatemap = unordered_map<size_t, pair< string , FileTemplate >> ;

class Node {					// build the tree..
private:
	static string kRootFilename;

	void addpage(Messages&,NodeFilename*);

//Generation
	void outputtofile(size_t page, string& out);

protected:
	Node *nodeparent;	  // parent
	size_t nodeid;        // unique id
	size_t nodetw;				// treewalk value
	size_t nodetier;			// tier of Node
	size_t nodesiblingnum;			// my number from 1 to n (NOT a zero prefixed array!)
	vector< Node* > children;		// list of children
	string idstr;

//reference to tree-type values (content,tax,suffix, etc)
	NodeVal* v;
	void  addPeers(vector<Node *>&,size_t);

public:
	static templatemap		templateList;
	static intintvectmap	layoutList;				//Stores a layout-id -> template-list structure
	static deque< Node* > 	node_stack;				//current node - used to pass to built-in functions

	static Node* roott;
	static Node* rootc;
	static Node* roots;

	static 	refmaptype tax_fields;								//A quick reference to the fields by name (as in iTax)
	enum kind { page, tax, file };
	NodeLocator* locator;

//construction functions
	Node();
	Node(NodeLocator*, NodeVal*,string,size_t,size_t,size_t);
	virtual ~Node();			//deleting a Node deletes all of it's children
	void addchild(Node* n);
	void reset();

	size_t getNumchildren();		//accessor

//Setup
	static void inittaxfields();
	static void loadLayouts(Messages&,Connection&);

//Generation
	void gettextoutput(Messages&);
	void generateoutput(int);
	void generatebranch(int);

//Nodetree navigation
	static Node* current(kind = page);
	static Node* node(size_t,Node::kind = Node::page); //by id.
	Node* parent()				{ return nodeparent; }
	Node* child(Messages&,size_t);
	Node* nodelast(Messages&,Node*);
	Node* nodenext(Messages&,Node*);
	Node* nodebyid(size_t);
	Node* nodebyidsilent(size_t); //stripped of error messages, so that we don't get dead parent errors during tree load
	Node* nodebytw(Messages&,signed long); //an offset from current node
	Node* nodebylinkref(Messages&,string);  //returns null if not content or suffix
	Node* nodebypath(Messages&,const string&);    //returns null if not found
	Node* nodebypath(Messages&,string::const_iterator,string::const_iterator);
	vector<Node *> siblings();
	vector<Node *> ancestors(Node* = nullptr);
	vector<Node *> peers(Node* = nullptr);
	bool isAncestor(Node* = nullptr);


//Data accessors.
	size_t id() const						{ return nodeid; }			// unique id
	string ids() const					{ return idstr; }		// unique id as a string
	size_t tw() const						{ return nodetw; }			// treewalk value
	size_t tier() const					{ return nodetier; }		// tier of Node
	size_t siblingnum() const		{ return nodesiblingnum; }	// my number from 1 to n (NOT a zero prefixed array!)
	size_t size() const;

//Accessors to nodeval data
	string& Filename(size_t i) const		{ return v->getfilename(i); }	//This is the initial page filename (includes . )
	string& Ffilename(size_t i ) const	{ return v->getffilename(i); }	//This is the full, final filename (post-processed)
	string& Suffix(size_t i) const		  { return v->getsuffix(i); }		//This is the initial page suffix.
	size_t Template(size_t i) const   	{ return v->gettemplate(i); }	// list of template ID's to use

	void Suffix(size_t, string);			//Set suffix at position.
	void SetFfilename(size_t, string);		//Set Ffilename at position.
	void SetFilename(size_t, string);		//Set Filename at position.

	void Layout(Messages&); //set the templates vector, resize, and add pages accordingly
	void setLayouts(Messages&); //Set all the layouts for a nodetree

	size_t filenameCount() const	{ return v->tplates.size(); }
	size_t ffilenameCount() const	{ return v->tplates.size(); }
	size_t suffixCount() const		{ return v->tplates.size(); }
	size_t templateCount() const	{ return v->tplates.size(); }

	void scratch(string s)	{ v->scratchpad= s;}			//Set scratchpad.
	string& scratch() const	{ return v->scratchpad; }	//get scratchpad.

	void basefilename(string s) { v->setsval(4,s);	}					//Set basefilename.
	void incPageCount()			{ v->setival(2, v->getival(2) +1 ); }	//increment pagecount (numpages)

	size_t tierdepth() const { return *(v->tierdepth); }	// my number from 1 to n (NOT a zero prefixed array!)

	size_t team()	 const	{ return v->getival(0); }
	size_t layout() const	{ return v->getival(1); }
	size_t numpages() const { return v->getival(2); }
	size_t exec()	const	{ return v->getival(0); }
	size_t batch() const	{ return v->getival(1); }

	string linkref() const		{ return v->getsval(0); }
	string title()	const		{ return v->getsval(1); }
	string shorttitle()	const	{ return v->getsval(2); }
	string tierref() const		{ return v->getsval(3); }
	string basefilename() const	{ return v->getsval(4); }
	string birthdate()	const	{ return v->getsval(5); }
	string deathdate()	const	{ return v->getsval(6); }

	string scope()	const		{ return v->getsval(0); }
	string classcode()	const	{ return v->getsval(3); }
	string synonyms() const		{ return v->getsval(4); }
	string keywords() const		{ return v->getsval(5); }
	string descr() const		{ return v->getsval(6); }

	string suffix() const		{ return v->getsval(0); }
	string script() const		{ return v->getsval(1); }
};

struct FileTemplate {
	string str;
	string suffix;
	string br;
};


#endif
