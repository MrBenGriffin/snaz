
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
	string Node::ref() 	 		 const	{ return _ref; }        	// unique id as a string
	string Node::ids() 	 		 const	{ return idStr; }        	// unique id as a string
	string Node::comm() 	 	 const	{ return _comment; }		// comment as a string

	//-------------------------------------------------------------------
	Node::Node(Tree& tree) : _tree(&tree),_parent(nullptr),_id(0),_tw(0),_tier(0),_weight(1),_sibling(1),idStr("") {
	}

	const Content* Node::content() const 	{ return dynamic_cast<const Content*>(this); }
	const Taxon* Node::taxon() const 		{ return dynamic_cast<const Taxon*>(this); }
	const Suffix* Node::suffix() const 		{ return dynamic_cast<const Suffix*>(this); }

	pair<valueField, vType> Node::field(Messages& e,const std::string& name) const {
		auto vf = gets[name];
		return { vf, vTypes[vf] };
	}

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
	
	Node::Node(Node && rhs) noexcept {
		_tree = rhs._tree;
		_parent = rhs._parent;
		_id = rhs._id;
		_tw = rhs._tw;
		_tier = rhs._tier;
		_sibling = rhs._sibling;
		_weight = rhs._weight;
		_ref=std::move(rhs._ref);
		idStr=std::move(rhs.idStr);
		_comment=std::move(rhs._comment);
		children=std::move(rhs.children);
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


	pair<size_t,deque<const Node *>> Node::siblings() const {
		pair<size_t,deque<const Node *>> result;
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

	deque<const Node *> Node::ancestors(const Node *anc) const {
		deque<const Node *> result;
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
	void Node::addToPeerList(pair<size_t,deque<const Node *>> &result,const Node *orig) const {
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

	pair<size_t,deque<const Node *>> Node::peers(const Node *anc) const {
		pair<size_t, deque<const Node *>> result;
		if (hasAncestor(anc)) {
			if(anc->_id != _id) {
				anc->addToPeerList(result, this); // All the nodes that are at my tier.
			} else {
				result = {0, {this}};
			};
		}
		return result;
	}

	void Node::setComment(std::string str) {
		_comment = std::move(str);
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

	bool Node::bGet(Messages &, valueField field) const {
		bool result = false;
		switch (field) {
			default:
				break;
		}
		return result;
	}

	size_t Node::iGet(Messages&, valueField field) const {
		size_t result = 0;
		switch (field) {
			case node::id:
				result = _id;
				break;
			case node::tw:
				result = _tw;
				break;
			case node::tier:
				result = _tier;
				break;
			case node::sibling:
				result = _sibling;
				break;
			case weight:
				result = _weight;
			default:
				break;
		}
		return result;
	}

	string Node::sGet(Messages&, valueField field) const {
		string result;
		switch (field) {
			case node::ref:
				result = _ref;
				break;
			case node::comment:
				result = _comment;
				break;
			default:
				break;
		}
		return result;
	}

	Date Node::dGet(Messages &, valueField field) const {
		Date result;
		switch (field) {
			default:
				break;
		}
		return result;
	}

//-------------------------------------------------------------------
// Contextual Navigation functions follow
//-------------------------------------------------------------------
	const Node* Node::child(Messages &errs, long long x) const { //This is 1-indexed!!
		if ( x <= 0 || x > children.size()) {
			ostringstream text;
			text << "A child index was outside the number of children. Index:" << x << " Children: " << children.size();
			errs << Message(range, text.str());
			return nullptr;
		}
		return children[x-1];
	}

//-------------------------------------------------------------------

}
