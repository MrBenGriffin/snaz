//
// Created by Ben on 2019-03-20.
//

#ifndef MACROTEXT_NODE_TREE_H
#define MACROTEXT_NODE_TREE_H

#include <map>
#include <unordered_map>
#include <string>

#include "support/Message.h"
#include "node/Locator.h"
#include "node/Node.h"

using namespace std;
using namespace Support;
namespace node {


	class Tree {
	private:
		string name;
		Tree() = delete;
		const Node *_root;
		Locator locator;

	public:
		map<size_t, const Node *> twNodes;
		unordered_map<string, const Node *> refNodes;
		size_t _depth;            // maximum depth of tree.
		size_t maxTw;            // maximum tree-walk; it should be the same as the size of the twNodes map!

		Tree(string);
		void clear();			 // empty the table.
		size_t size() const { return twNodes.size(); }
		size_t depth() const { return _depth; }

		const Node* current() const;
		const Node* root() const;
		const Node* node(Messages &, size_t, bool= false) const; //by id.
		const Node* next(Messages &, size_t) const; //treeWalk next.
		const Node* prev(Messages &, size_t) const; //treeWalk previous
		const Node* byRef(Messages &, string, bool= true) const;  //returns null if not content or suffix
		const Node* byPath(Messages &,const string &);    //returns null if not found

		const Node* tw(Messages &, size_t, signed long,const Node* = nullptr) const; //treeWalk from offset.
		void add(Messages &, Node*, size_t);

	};
}
#endif //MACROTEXT_NODE_TREE_H
