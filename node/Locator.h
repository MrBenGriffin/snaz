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

using namespace std;
using namespace Support;
namespace node {

	class Node;
	struct Metrics;

	class Locator {

	private:
		string::const_iterator start;
		string::const_iterator in;
		string::const_iterator out;

		typedef bool (Locator::*Fn)(Support::Messages &); //A fn returns false if it failed, otherwise true.

		size_t pageNum;
		const Metrics *metrics;
		const Node *find;
		const Node *from;
		const Node *root;

		bool nextPathSection(Messages &);

		ostringstream message;
		Locator();
		Locator(const Metrics *,const Node*,const Node*);

	public:
		static bool showPaths;

		stack<bool> _dirty;

		Locator(const Metrics *);

		void setFrom(const Node*);
		void setRoot(const Node*);

		void setdirty();

		void track() { _dirty.push(false); }

		bool tracking() { return !_dirty.empty(); }

		bool untrack() {
			auto status = _dirty.top();
			_dirty.pop();
			return status;
		}

		size_t getFoundPageNumber() { return pageNum; }

		const Node* get(Support::Messages&,const string&);

		const Node *locate(Support::Messages &, string::const_iterator, string::const_iterator);

		void process(Support::Messages &, string::const_iterator &in, string::const_iterator &out);

		//TextAdaptor Functions
		bool doID(Support::Messages &);
		bool doParent(Support::Messages &);
		bool doPeerNext(Support::Messages &);
		bool doPeerLast(Support::Messages &);
		bool doRR(Support::Messages &);
		bool doSiblingRight(Support::Messages &);
		bool doSiblingLeft(Support::Messages &);
		bool doPage(Support::Messages &);
		bool doMainNode(Support::Messages &);
		bool doChildLeft(Support::Messages &);
		bool doChildRight(Support::Messages &);
		bool doLinkRef(Support::Messages &);
		bool doIllegalCharacter(Support::Messages &);
		bool doAncestorAbsolute(Support::Messages &);
		bool doAncestorRelative(Support::Messages &);
		bool doTaxonomy(Support::Messages &);
		bool doContent(Support::Messages &);
		bool doSuffix(Support::Messages &);
		bool doStackFromInside(Support::Messages &);
		bool doStackFromOutside(Support::Messages &);
		bool doTW(Support::Messages &);

	private:
		static array<Fn, 256> fns; // = {&Locator::doIllegalCharacter};
		static bool fnSet; // = {&Locator::doIllegalCharacter};

	};
}
#endif //MACROTEXT_NODELOCATOR_H
