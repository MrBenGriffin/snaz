#ifndef MACROTEXT_NODE_NODE_H
#define MACROTEXT_NODE_NODE_H

#include <string>
#include <utility>
#include <deque>
#include <variant>
#include <map>
#include <sstream>

#include "support/Definitions.h"
#include "support/Message.h"
#include "support/Date.h"
#include "support/db/Connection.h"
#include "support/db/Query.h"

using namespace Support;

namespace node {

	class Tree;
	class Node;
	class NodeFilename;
	class Locator;
	class Content;
	class Taxon;
	class Suffix;

	enum valueField : unsigned int {
		exec=100,batch=101,terminal=102,evaluate=103,container=104,
		team=200,layout=201,templates=202,tier=203,id=204,tw=205,sibling=206,weight=207,
		title=300,shortTitle=301,comment=302,baseFilename=303,scope=304,classCode=305,synonyms=306,
		keywords=307,description=308,fileSuffix=309,script=310,editor=311,ref=312,
		modified=401,birth=402,death=403
	};
	enum flavour    { content,taxon,suffix};
	enum vType : unsigned int 		{ vBool = 100, vNumber = 200, vString = 300,  vDate = 400 };

	static unordered_map<string,valueField> gets({
			{"exec",exec},{"batch",batch},{"terminal",terminal},{"evaluate",evaluate},{"container",container},
			{"team",team},{"layout",layout},{"templates",templates},{"tier",tier},{"id",id},{"tw",tw},{"sibling",sibling},{"weight",weight},
			{"title",title},{"shortTitle",shortTitle},{"comment",comment},{"baseFilename",baseFilename},{"scope",scope},
			{"classCode",classCode},{"synonyms",synonyms},{"keywords",keywords},
			{"description",description},{"fileSuffix",fileSuffix},{"script",script},{"editor",editor},{"ref",ref},
			{"modified",modified},{"birth",birth},{"death",death},
// Synonyms
			{"linkref",ref},{"shorttitle",shortTitle},{"classcode",classCode},{"article",description}
	});

	static unordered_map<valueField, vType> vTypes({
		   {exec,vBool},{batch,vBool},{terminal,vBool},{evaluate,vBool},{container,vBool},
		   {team,vNumber},{layout,vNumber},{templates,vNumber},{tier,vNumber},{id,vNumber},{tw,vNumber},{sibling,vNumber},{weight,vNumber},
		   {title,vString},{shortTitle,vString},{comment,vString},{baseFilename,vString},{scope,vString},{classCode,vString},{synonyms,vString},
		   {keywords,vString},{description,vString},{fileSuffix,vString},{script,vString},{editor,vString},{ref,vString},
		   {modified,vDate},{birth,vDate},{death,vDate}
	});

	class Node {                    // build the tree..
	private:

		void addToPeerList(pair<size_t,deque<const Node *>> &, const Node*) const; //internal recursion function used by peers().

	protected:
		Tree* _tree;						// The tree I belong to.
		Node* _parent;      		// parent
		size_t _id;        			// unique id
		size_t _tw;             // treewalk value
		size_t _tier;           // tier of Node
		size_t _sibling;        // my number from 1 to n (NOT a zero prefixed array!) - this can be derived from where I am in my parents children.
		size_t _weight;					// size of my branch.
		string _ref;						// Unique reference string (eg, LinkRef.
		string idStr;						// As we get this from the database, we may as well store the string 'for free'.
		string _comment;				// comment, used by content as 'scratch'.
		deque<const Node *> children;      // List of children
		void common(Messages&,Db::Query*,size_t&,size_t&);

	public:
		Node() = delete;
		Node(Node &&) noexcept; //ensure that we do not use a copy constructor on move..
		explicit Node(Tree&);
		virtual flavour cultivar() const = 0;

//construction functions
		virtual ~Node();            //deleting a Node deletes all of it's children
		virtual const Node* node(Messages&, size_t, bool= false) const = 0; //by id.
		virtual void loadTree(Messages&, Db::Connection&, size_t = 0, buildKind = final) = 0; //depends upon Node flavour of

//Setup

//Node-tree (within a node context).
		void addChild(Messages&,Node*);
		size_t getChildCount() const;        //accessor
		deque<const Node *> ancestors(const Node* = nullptr) const;
		pair<size_t,deque<const Node *>> siblings() const;
		pair<size_t,deque<const Node *>> peers(const Node* = nullptr) const;

		void  setComment(std::string);
		const Node* parent() const { return _parent; }
		const Tree* tree() const { return _tree; }
		const Node* child(Messages&, long long) const;
		bool hasAncestor(const Node* = nullptr) const;
		const Node* offset(Messages&, signed long) const; 		//a tw offset from current node

		//Data accessors.
		size_t id() const;	 		// unique id
		size_t tw() const;	 		// tree-walk value
		size_t tier() const;	 	// my tier number from 1 to n
		size_t sibling() const;		// my sibling number from 1 to n
		size_t size() const;
		string ids() const;			// unique id as a string
		string ref() const;			// linkref as a string
		string comm() const;		// comment as a string

		void str(ostream&) const;			 // show the Node..
		virtual void weigh();

		const Content* content() const; //{ return dynamic_cast<const Content*>(this); }
		const Taxon* taxon() const ;	//{ return dynamic_cast<const Taxon*>(this); }
		const Suffix* suffix() const; //{ return dynamic_cast<const Suffix*>(this); }

		//Accessors to Value data
		//bool, size_t, string, Date
		pair<valueField, vType> field(Messages&,const std::string&) const;

		virtual bool   bGet(Messages&,valueField) const;
		virtual size_t iGet(Messages&,valueField) const;
		virtual string sGet(Messages&,valueField) const;
		virtual Date   dGet(Messages&,valueField) const;

	};

}


#endif // MACROTEXT_NODE_H
