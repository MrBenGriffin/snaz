//
// Created by Ben on 2019-03-20.
//
#include <algorithm>
#include <sstream>

#include "Tree.h"
#include "Node.h"

#include "support/Message.h"

namespace node {

//-------------------------------------------------------------------
	Tree::Tree(string _name) : _root(nullptr),name(_name),depth(0),maxTw(0),twNodes(),refNodes() {}
//-------------------------------------------------------------------
	void Tree::clear() {
		_root = nullptr;
		twNodes.clear();
		refNodes.clear();
		depth = 0;
		maxTw = 0;
	}
//-------------------------------------------------------------------
	void Tree::add(Messages &errs, Node* node, size_t _parent) {
		if(_parent == 0) {
			_root = node;
		} else {
			if(_root != nullptr) {
				const Node* parent = _root->node(errs,_parent);
				if(parent != nullptr) {
					const_cast<Node*>(parent)->addChild(errs,node);
				} else {
					ostringstream err;
					err << "The tree `" << name << "` could not find the parent " << _parent << " for the node " << node->ref();
					errs << Message(error,err.str());
				}
			} else {
				ostringstream err;
				err << "The tree `" << name << "` must have it's root node set before any other node.";
				errs << Message(error,err.str());
			}
		}
		auto tw = node->tw();
		twNodes.emplace(tw,node);
		refNodes.emplace(node->ref(),node);
		depth = std::max(depth,node->tier());
		maxTw = std::max(maxTw,tw);
	}
//-------------------------------------------------------------------
	const Node* Tree::current() const {
		return _root->current();
	}
	const Node* Tree::root() const { return _root; }
//-------------------------------------------------------------------
	const Node* Tree::node(Messages &errs,size_t id,bool silent) const {
		if(_root == nullptr) {
			return nullptr;
		} else {
			return _root->node(errs,id,silent);
		}
	}
//-------------------------------------------------------------------
	const Node* Tree::tw(Messages& errs,size_t tw,signed long offset,const Node* localRoot) const {
		const Node* result =  nullptr;
		if(tw > maxTw || tw < 1) {
			ostringstream err;
			err << "The tree `" << name << "` was not scoped by the tree-walk value " << tw;
			errs << Message(range,err.str());
		} else {
			tw = (tw + offset) % maxTw + 1;
			if(tw == 0) { tw = maxTw; }
			auto found = twNodes.find(tw);
			if(found != twNodes.end()) {
				result = found->second;
			} else {
				ostringstream err;
				err << "Weirdly, the tree `" << name << "` could not find a node with the tree-walk value " << tw;
				errs << Message(error,err.str());
			}
		}
		return result;
	}
	const Node* Tree::next(Messages &errs,size_t pos) const {
		return tw(errs,pos,+1);
	}
	const Node* Tree::prev(Messages &errs,size_t pos) const {
		return tw(errs,pos,-1);
	}
//-------------------------------------------------------------------
	const Node* Tree::byRef(Messages &errs, const string ref,bool silent) const {
		const Node *result = nullptr;
		auto found = refNodes.find(ref);
		if(found != refNodes.end()) {
			result = found->second;
		} else {
			if(!silent) {
				ostringstream err;
				err << "Tree " << name << " cannot find a node with reference " << ref;
				errs << Message(range,err.str());
			}
		}
		return result;
	}
//-------------------------------------------------------------------
/*
	return the Node described by 'path'
	 "/TCS!:n+-.A^RFBIO0123456789" every legal initial char
	 "/TS!:AI123456789" absolute (for node) initial chars
	 "C0n+-.^RFBO" relative (for node) initial chars
*/
	const Node* Tree::byPath(Messages &errs,Locator& locator, const string &path) const {
		const Node *result = nullptr;
		if ((path.size() > 0) && (_root->cultivar() == content)) {    // If specified startnode, use it
			const string relatives = "C0n+-.^RFBO"; //yes, 0 is a relative address in this case!
			if (relatives.find(path[0]) != string::npos) {
				locator.setdirty();
			} else {
				if (path.length() > 1) {
					if (path[0] == 'I' && path[1] != '0') {
						locator.setdirty();
					} else {
						if (path[0] == 'A' && path[1] != '1') {
							locator.setdirty();
						}
					}
				}
			}
		}
		locator.setFrom(current());
		result = locator.locate(errs,path.begin(),path.end());
		if (result == nullptr) {
			errs << Message(error, "Path: " + path + " did not find a node.");
		}
		return result;
	}
//-------------------------------------------------------------------
//return the Node described by 'path'
	const Node* Tree::byPath(Messages &errs,Locator& locator, string::const_iterator in, string::const_iterator out) const {
		const Node *result = nullptr;
		if ((out - in) > 0) {    // If specified startnode, use it
			locator.setFrom(current());
			result = locator.locate(errs, in, out);
		}
		return result;
	}

}


