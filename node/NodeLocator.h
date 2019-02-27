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
	typedef bool (NodeLocator::*Fn)(); //A fn returns false if it failed, otherwise true.

	size_t pageNum;
	Node *find;
	Node *from;

	bool nextPathSection();
	Support::Messages* errs;
	ostringstream message;

public:
	static bool showPaths;
	static string loc_path;
	Node *root;
	stack<bool> _dirty;
	NodeLocator();

	void setdirty();
	void track() { _dirty.push(false);  }
	bool tracking() { return !_dirty.empty(); }
	void setroot(Node *newroot) { root = newroot; }
	bool untrack() { auto status = _dirty.top(); _dirty.pop(); return status; }
	size_t getFoundPageNumber() { return pageNum; }
	Node* locate(Support::Messages&,Node*,string::const_iterator,string::const_iterator);
	bool dostop();
	void processinput(string::const_iterator& in,string::const_iterator& out);

	//TextAdaptor Functions
	bool doID();
	bool doParent();
	bool doPeerNext();
	bool doPeerLast();
	bool doRR();
	bool doSiblingRight();
	bool doSiblingLeft();
	bool doPage();
	bool doMainNode();
	bool doChildLeft();
	bool doChildRight();
	bool doLinkRef();
	bool doIllegalCharacter();
	bool doAncestorAbsolute();
	bool doAncestorRelative();
	bool doTaxonomy();
	bool doContent();
	bool doSuffix();
	bool doStackFromInside();
	bool doStackFromOutside();

private:
	array< Fn, 256>fns = {&NodeLocator::doIllegalCharacter};

};

#endif //MACROTEXT_NODELOCATOR_H
