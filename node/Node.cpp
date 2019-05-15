
#include <sstream>
#include <utility>
#include <iomanip>

#include "node/Node.h"
#include "node/Tree.h"
#include "node/Locator.h"
#include "node/NodeFilename.h"

//#include "node/Content.h"
//#include "node/Taxon.h"

//#include "mt/Driver.h"
#include "support/File.h"
//#include "support/Regex.h"
//#include "support/Convert.h"
#include "support/Env.h"
#include "support/Timing.h"
#include "Build.h"
#include "node/Content.h"
#include "node/Suffix.h"
#include "node/Taxon.h"

using namespace Support;

namespace node {

//-------------------------------------------------------------------
//Accessors
	size_t Node::id() 			 const	{ return _id; }        		// unique id
	size_t Node::tw() 			 const	{ return _tw; }        		// treewalk value
	size_t Node::tier() 		 const	{ return _tier; }    		// tier of Node
	size_t Node::sibling() 	 	 const	{ return _sibling; }    	// my number from 1 to n (NOT a zero prefixed array!)
	size_t Node::size() 		 const 	{ return _weight; }
	size_t Node::getChildCount() const  { return children.size(); }
	const string Node::ref() 	 const	{ return _ref; }        	// unique id as a string
	const string Node::ids() 	 const	{ return idStr; }        	// unique id as a string
//-------------------------------------------------------------------
	Node::Node(Tree& tree) : _tree(&tree),_parent(nullptr),_id(0),_tw(0),_tier(0),_weight(1),_sibling(1),idStr("") {
	}

	const Content* Node::content() const 	{ return dynamic_cast<const Content*>(this); }
	const Taxon* Node::taxon() const 		{ return dynamic_cast<const Taxon*>(this); }
	const Suffix* Node::suffix() const 		{ return dynamic_cast<const Suffix*>(this); }

	//-------------------------------------------------------------------
	//Read fields common to all nodes here.
	void Node::common(Messages& errs,Db::Query* q,size_t& parent,size_t& tw) {
		//weights will be calculated later.
		q->readfield(errs,"id",idStr);
		q->readfield(errs,"id",_id);
		q->readfield(errs,"parent",parent); 		// parent is stored as a node. will be calculated by add.
		q->readfield(errs,"tier",_tier);
		q->readfield(errs,"ref",_ref);
		if(tw == 0) {
			q->readfield(errs,"tw",_tw); 			// we can use this because there are no 'unused' nodes.
		} else {
			_tw = tw++;
		}
	}

	//-------------------------------------------------------------------
	void Node::addChild(Messages&,Node* node) {
		node->_sibling = children.size() +1;
		node->_parent = this;
		children.push_back(node);
	}

	//-------------------------------------------------------------------
	const Node* Node::offset(Messages &errs, signed long offset) const { //root isn't in here.. 0 will return end of tree.
		return _tree->tw(errs,_tw,offset);
	}

	//-------------------------------------------------------------------
	Node::~Node() {
		children.clear();
		_tree = nullptr; 	// a reference value, so do not delete here!
		_parent = nullptr;  // a reference value - so do not delete here!
	}


	pair<size_t,vector<const Node *>> Node::siblings() const {
		pair<size_t,vector<const Node *>> result;
		if(_parent != nullptr) {
			for (auto kid: _parent->children) {
				if(kid->_id == _id) { //zero-indexed position of self
					result.first = result.second.size();
				}
				result.second.push_back(kid);
			}
		} else {
			result.first = 0; //zero-indexed position of self
			result.second.push_back(this);
		}
		return result;
	}

	bool Node::hasAncestor(const Node *anc) const {
		return anc
			   && _tw >= anc->_tw
			   && _tw <  (anc->_tw+anc->_weight);
	}

	vector<const Node *> Node::ancestors(const Node *anc) const {
		vector<const Node *> result;
		const Node *cursor = this;
		while ((cursor != anc) && cursor->_parent != nullptr) {
			result.push_back(cursor);
			cursor = cursor->_parent;
		}
		if (cursor == anc || (cursor && !anc)) {
			result.push_back(cursor);
		}
		return result;
	}

	//internal recursive function used to gather nodes at a given depth...
	void Node::addToPeerList(pair<size_t,vector<const Node *>> &result,const Node *orig) const {
		if ( orig->_tier == _tier + 1) {
			for (auto kid: children) {
				if(kid->_id == orig->_id) {  //zero-indexed position of self
					result.first = result.second.size();
				}
				result.second.push_back(kid);
			}
		} else {
			for (auto kid: children) {
				kid->addToPeerList(result,orig);
			}
		}
	}

	pair<size_t,vector<const Node *>> Node::peers(const Node *anc) const {
		pair<size_t, vector<const Node *>> result;
		if (hasAncestor(anc)) {
			if(anc->_id != _id) {
				anc->addToPeerList(result, this); // All the nodes that are at my tier.
			} else {
				result = {0, {this}};
			};
		}
		return result;
	}

	void Node::str(ostream& o) const {
		std::setfill('-');
		o << std::setw(_tier + 6) << " tier:" << _tier << " weight:" << _weight << " tw:" << _tw  << " id:" << _id << " ref:" << _ref << endl;
		for (auto& i : children) {
			i->str(o);
		}
	}

	void Node::weigh() {
		_weight = 1;
		for (auto& i : children) {
			const_cast<Node*>(i)->weigh();
			_weight += i->_weight;
		}
	}

//-------------------------------------------------------------------
// Contextual Navigation functions follow
//-------------------------------------------------------------------
	const Node* Node::child(Messages &errs, size_t x) const { //This is 1-indexed!!
		if ( x == 0 || x > children.size()) {
			ostringstream text;
			text << "A child index outside the number of children. Index:" << x << " Children: " << children.size();
			errs << Message(range, text.str());
			return nullptr;
		}
		return children[x-1];
	}

//-------------------------------------------------------------------

}
