//
// Created by Ben on 2019-04-24.
//

#include "Metrics.h"

#include "content/Template.h"
#include "content/Segment.h"

#include "node/Content.h"
#include "node/Node.h"
#include "node/Locator.h"


namespace node {

	using namespace Support;
	Metrics::Metrics() : current(nullptr),currentTemplate(nullptr),page(0) {
		locator =  std::make_shared<Locator>(this);
		locator->setRoot(Content::root());
	}
	Metrics::~Metrics() {
	}

	//		void push(const Content*,const Segment* = nullptr);
	void Metrics::push(const Content* node,const content::Segment* segment){
		nodeStack.push_back(node);
		current  = node;
		segmentStack.push(segment);
		mt::Wss::push(&(segment->nl));
	}
	void Metrics::pop() {
		segmentStack.pop();
		mt::Wss::pop();
		nodeStack.pop_back();
		current = nodeStack.back();
	}

	Metrics::Metrics(const Metrics* o) : locator(nullptr) {
		current = o->current;
        locator =  std::make_shared<Locator>(o->locator.get(),this);
		currentTemplate = o->currentTemplate;
		segmentStack = o->segmentStack;
		nodeStack = o->nodeStack;
		page = o->page;
	}

	const Node* Metrics::byPath(Messages &errs,const std::string &path) const {
		const Node *result = nullptr;
		if (path.size() > 0) {    // If specified startnode, use it
			const string relatives = "C0n+-.^RFBO"; //yes, 0 is a relative address in this case!
			if (relatives.find(path[0]) != string::npos) {
				locator->setdirty();
			} else {
				if (path.length() > 1) {
					if (path[0] == 'I' && path[1] != '0') {
						locator->setdirty();
					} else {
						if (path[0] == 'A' && path[1] != '1') {
							locator->setdirty();
						}
					}
				}
			}
			locator->setFrom(current);
			result = locator->locate(errs,path.begin(),path.end());
			if (result == nullptr) {
				errs << Message(error, "Path: " + path + " did not find a node.");
			}
			locator->setRoot(Content::root());
		} else {
			result = current;
		}
		return result;
	}

	pair<const Node*,size_t> Metrics::nodePage(Messages &errs,const std::string &path) const {
		pair<const Node*,size_t> result = {nullptr,0};
		if (path.size() > 0) {    // If specified startnode, use it
			const string relatives = "C0n+-.^RFBO"; //yes, 0 is a relative address in this case!
			if (relatives.find(path[0]) != string::npos) {
				locator->setdirty();
			} else {
				if (path.length() > 1) {
					if (path[0] == 'I' && path[1] != '0') {
						locator->setdirty();
					} else {
						if (path[0] == 'A' && path[1] != '1') {
							locator->setdirty();
						}
					}
				}
			}
		}
		locator->setFrom(current);
		result.first = locator->locate(errs,path.begin(),path.end());
		result.second = locator->getFoundPageNumber();
		if (result.first == nullptr) {
			errs << Message(error, "Path: " + path + " did not find a node.");
		}
		if (result.second == UINTMAX_MAX) {
			errs << Message(range, "Page was out of bounds.");
		}
		locator->setRoot(Content::root());
		return result;
	}

}