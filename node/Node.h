#ifndef MACROTEXT_NODE_NODE_H
#define MACROTEXT_NODE_NODE_H

#include <string>
#include <utility>
#include <vector>
#include <deque>
#include <map>
#include <sstream>

#include "support/Message.h"
#include "support/Date.h"

using namespace Support;

namespace node {

	class Tree;
	class Node;
	class NodeFilename;
	class Locator;

//		size_t team() const;
//		size_t layout() const;
//		size_t numpages() const;
//		size_t exec() const;
//		size_t batch() const;
//		string linkref() const;
//		string title() const;
//		string shorttitle() const;
//		string tierref() const;
//		string basefilename() const;
//		string birthdate() const;
//		string deathdate() const;
//		string scope() const;
//		string classcode() const;
//		string synonyms() const;
//		string keywords() const;
//		string descr() const;
//		string suffix() const;
//		string script() const;

	enum boolValue  { exec,batch};
	enum uintValue  { team,layout,page,templates};
	enum textValue  { title,shortTitle,tierRef,baseFilename,scope,classCode,synonyms,keywords,descr,fileSuffix,script};
	enum dateValue  { birth,death};

	enum flavour { content,taxon,suffix};

	class Node {                    // build the tree..
	private:
		Node();

		void addToPeerList(vector<const Node *> &, size_t) const; //internal recursion function used by peers().

	protected:
		Tree* _tree;				// The tree I belong to.
		Node* _parent;      		// parent
		size_t _id;        			// unique id
		size_t _tw;                	// treewalk value
		size_t _tier;            	// tier of Node
		size_t _sibling;         	// my number from 1 to n (NOT a zero prefixed array!) - this can be derived from where I am in my parents children.
		size_t _weight;				// size of my branch.
		string _ref;				// Unique reference string (eg, LinkRef.
		string idStr;				// As we get this from the database, we may as well store the string 'for free'.
		vector<Node *> children;    // List of children

	public:
		virtual flavour cultivar() const = 0;

//construction functions
		Node(Locator*, string, size_t, size_t, size_t);
		virtual ~Node();            //deleting a Node deletes all of it's children
		virtual Node* current() const = 0; //depends upon Node flavour of

//Setup

//Node-tree (within a node context).
		void addChild(Messages&,Node*);
		size_t getChildCount() const;        //accessor
		vector<const Node *> siblings() const;
		vector<const Node *> ancestors(const Node* = nullptr) const;
		vector<const Node *> peers(const Node* = nullptr) const;

		const Node* parent() const { return _parent; }
		const Tree* tree() const { return _tree; }
		const Node* child(Messages&, size_t) const;
		bool isAncestor(const Node* = nullptr) const;
		const Node* offset(Messages&, signed long) const; //a tw offset from current node

		//Data accessors.
		size_t id() const;	 		// unique id
		size_t tw() const;	 		// tree-walk value
		size_t tier() const;	 	// my tier number from 1 to n
		size_t sibling() const;		// my sibling number from 1 to n
		size_t size() const;
		const string ids() const;	 		// unique id as a string
		const string ref() const;	 		// linkref as a string

//Accessors to Value data
		virtual bool   get(Messages&,boolValue) const = 0;
		virtual size_t get(Messages&,uintValue) const = 0;
		virtual string get(Messages&,textValue) const = 0;
		virtual Date   get(Messages&,dateValue) const = 0;


//		enum boolValue  { exec,batch};
//		enum uintValue  { team,layout,pages};
//		enum textValue  { title,shortTitle,tierRef,baseFilename,scope,classCode,synonyms,keywords,descr,fileSuffix,script};
//		enum dateValue  { birth,death};



//		size_t team() const;
//		size_t layout() const;
//		size_t numpages() const;
//		size_t exec() const;
//		size_t batch() const;
//		string linkref() const;
//		string title() const;
//		string shorttitle() const;
//		string tierref() const;
//		string basefilename() const;
//		string birthdate() const;
//		string deathdate() const;
//		string scope() const;
//		string classcode() const;
//		string synonyms() const;
//		string keywords() const;
//		string descr() const;
//		string suffix() const;
//		string script() const;
	};

}


#endif // MACROTEXT_NODE_H
