
#include <sstream>
#include <utility>
#include <iomanip>

#include "node/Node.h"
#include "node/Tree.h"
#include "node/Locator.h"
#include "node/NodeFilename.h"

//#include "node/Content.h"
//#include "node/Taxon.h"
//#include "node/Suffix.h"

//#include "mt/Driver.h"
#include "support/File.h"
//#include "support/Regex.h"
//#include "support/Convert.h"
#include "support/Env.h"
#include "support/Timing.h"
#include "Build.h"

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
	Node::Node(Tree& tree) : _tree(&tree),_parent(nullptr),_id(0),_tw(0),_tier(0),_weight(1),_sibling(0),idStr("") {
	}

	//-------------------------------------------------------------------
	//Read fields common to all nodes here.
	void Node::common(Messages& errs,Db::Query* q,size_t& parent,size_t& tw) {
		//weights will be calculated later.
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
	void Node::addChild(Messages & errs,Node* node) {
		node->_sibling = children.size();
		node->_parent = this;
		children.push_back(node);
	}

	//-------------------------------------------------------------------
	const Node* Node::offset(Messages &errs, signed long offset) const { //root isn't in here.. 0 will return end of tree.
		return _tree->tw(errs,_tw,offset);
	}

	//-------------------------------------------------------------------
	Node::~Node() {
//		while (!children.empty()) {
//			delete children.back();
//			children.pop_back();
//		}
		children.clear();
		_tree = nullptr; 	// a reference value, so do not delete here!
		_parent = nullptr;  // a reference value - so do not delete here!
	}

	vector<const Node *> Node::siblings() const {
		vector<const Node *> result;
		for (auto kid: _parent->children) {
			result.push_back(kid);
		}
		return result;
	}

	bool Node::isAncestor(const Node *anc) const {
		const Node *cursor = this;
		while (cursor && anc && cursor != anc && cursor->_parent != nullptr) {
			cursor = cursor->_parent;
		}
		return anc && cursor == anc;
	}

	vector<const Node *> Node::ancestors(const Node *anc) const {
		const Node *cursor = this;
		vector<const Node *> result;
		while ((cursor != anc) && cursor->_parent != nullptr) {
			result.push_back(cursor);
			cursor = cursor->_parent;
		}
		if (cursor == anc) {
			result.push_back(anc);
		}
		return result;
	}

//internal recursive function used to gather nodes at a given depth...
	void Node::addToPeerList(vector<const Node *> &result, size_t depth) const {
		if (depth < 1) return;
		for (auto kid: children) {
			if (depth == 1) {
				result.push_back(kid);
			} else {
				kid->addToPeerList(result, depth - 1);
			}
		}
	}

	vector<const Node *> Node::peers(const Node *anc) const {
		if (isAncestor(anc)) {
			vector<const Node *> result;
			anc->addToPeerList(result, _tier - anc->_tier); //6-5 == 1 so it should return siblings..
			return result;
		} else {
			return siblings();
		}
	}

	void Node::str(ostream& o) const {
		std::setfill('-');
		o << std::setw(6+ _tier) << " tier:" << _tier << " weight:" << _weight << " tw:" << _tw  << " id:" << _id << " ref:" << _ref << endl;
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
	 const Node* Node::child(Messages &errs, size_t x) const {
		if (x >= children.size()) {
			ostringstream text;
			text << "A child index was greater than the number of children. Index:" << x << " Children: " << children.size();
			errs << Message(error, text.str());
			return nullptr;
		}
		return children[x];
	}



//-------------------------------------------------------------------

//	string &Node::Suffix(size_t i) const { return value->getsuffix(i); }        //This is the initial page suffix.
//	size_t Node::Template(size_t i) const { return value->gettemplate(i); }    // list of template ID's to use
//
//	size_t Node::filenameCount() const { return value->tplates.size(); }
//
//	size_t Node::ffilenameCount() const { return value->tplates.size(); }
//
//	size_t Node::suffixCount() const { return value->tplates.size(); }
//
//	size_t Node::templateCount() const { return value->tplates.size(); }
//
//	void Node::scratch(string s) { value->scratchpad = s; }            //Set scratchpad.
//	string &Node::scratch() const { return value->scratchpad; }    //get scratchpad.
//
//	void Node::basefilename(string s) { value->setsval(4, s); }                    //Set basefilename.
//	void Node::incPageCount() { value->setival(2, value->getival(2) + 1); }    //increment pagecount (numpages)
//
//	size_t Node::team() const { return value->getival(0); }
//
//	size_t Node::layout() const { return value->getival(1); }
//
//	size_t Node::numpages() const { return value->getival(2); }
//
//	size_t Node::exec() const { return value->getival(0); }
//
//	size_t Node::batch() const { return value->getival(1); }
//
//	string Node::linkref() const { return value->getsval(0); }
//
//	string Node::title() const { return value->getsval(1); }
//
//	string Node::shorttitle() const { return value->getsval(2); }
//
//	string Node::tierref() const { return value->getsval(3); }
//
//	string Node::basefilename() const { return value->getsval(4); }
//
//	string Node::birthdate() const { return value->getsval(5); }
//
//	string Node::deathdate() const { return value->getsval(6); }
//
//	string Node::scope() const { return value->getsval(0); }
//
//	string Node::classcode() const { return value->getsval(3); }
//
//	string Node::synonyms() const { return value->getsval(4); }
//
//	string Node::keywords() const { return value->getsval(5); }
//
//	string Node::descr() const { return value->getsval(6); }
//
//	string Node::suffix() const { return value->getsval(0); }
//
//	string Node::script() const { return value->getsval(1); }
}