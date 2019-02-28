//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_NODELOCATOR_H
#define MACROTEXT_NODELOCATOR_H


#include <map>
#include <vector>
#include <array>
#include <stack>

#include "support/Message.h"
#include "Node.h"

using namespace std;

class NodeLocator {

private:
	string::const_iterator start;
	string::const_iterator in;
	string::const_iterator out;
	typedef bool (NodeLocator::*Fn)(Support::Messages&); //A fn returns false if it failed, otherwise true.

	size_t pageNum;
	Node *find;
	Node *from;

	bool nextPathSection(Messages&);
	ostringstream message;

public:
	static bool showPaths;
//	static string loc_path;
	Node *root;
	stack<bool> _dirty;
	NodeLocator();

	void setdirty();
	void track() { _dirty.push(false);  }
	bool tracking() { return !_dirty.empty(); }
	bool untrack() { auto status = _dirty.top(); _dirty.pop(); return status; }
	void setroot(Node *newroot) { root = newroot; }
	size_t getFoundPageNumber() { return pageNum; }
	Node* locate(Support::Messages&,Node*,string::const_iterator,string::const_iterator);
//	bool dostop();
	void processinput(Support::Messages&,string::const_iterator& in,string::const_iterator& out);

	//TextAdaptor Functions
	bool doID(Support::Messages&);
	bool doParent(Support::Messages&);
	bool doPeerNext(Support::Messages&);
	bool doPeerLast(Support::Messages&);
	bool doRR(Support::Messages&);
	bool doSiblingRight(Support::Messages&);
	bool doSiblingLeft(Support::Messages&);
	bool doPage(Support::Messages&);
	bool doMainNode(Support::Messages&);
	bool doChildLeft(Support::Messages&);
	bool doChildRight(Support::Messages&);
	bool doLinkRef(Support::Messages&);
	bool doIllegalCharacter(Support::Messages&);
	bool doAncestorAbsolute(Support::Messages&);
	bool doAncestorRelative(Support::Messages&);
	bool doTaxonomy(Support::Messages&);
	bool doContent(Support::Messages&);
	bool doSuffix(Support::Messages&);
	bool doStackFromInside(Support::Messages&);
	bool doStackFromOutside(Support::Messages&);

private:
	array< Fn, 256>fns = {&NodeLocator::doIllegalCharacter};

};

#endif //MACROTEXT_NODELOCATOR_H
