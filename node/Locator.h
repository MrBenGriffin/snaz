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

		size_t pageNum;
		const Metrics *metrics;
		const Node *find;
		const Node *from;
		const Node *root;

		bool nextPathSection(Messages &);
		const Node* peerPrep(Messages &);

		ostringstream message;
		Locator();
		Locator(const Metrics *,const Node*,const Node*);
		void process(Support::Messages &);

	public:
		Locator(const Locator *,const Metrics*);
		static bool showPaths;

		stack<bool> _dirty;

		explicit Locator(const Metrics *);

		void setFrom(const Node*);
		void setRoot(const Node*);
		void setMetrics(const Metrics *m) {this->metrics = m;}

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

		const Node *locate(Support::Messages&,string::const_iterator,string::const_iterator);

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

	};
}
#endif //MACROTEXT_NODELOCATOR_H
