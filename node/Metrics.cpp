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
		locator =  std::make_unique<Locator>(this);
		locator->setRoot(Content::root());
	}
	Metrics::~Metrics() {
	}

	void Metrics::push(const Content* node,const content::Segment* segment){
		nodeStack.push_back(node);
		current  = node;
		auto segNull = segment == nullptr;
		nullStack.push(segNull);
		if(!segNull) {
			segmentStack.push(segment);
			mt::Wss::push(&(segment->nl));
		}
	}
	void Metrics::pop() {
		auto segNull = nullStack.top();
		nullStack.pop();
		if(!segNull) {
			segmentStack.pop();
			mt::Wss::pop();
		}
		nodeStack.pop_back();
		current = nodeStack.empty() ?
			nullptr :
			nodeStack.back();
	}

	Metrics::Metrics(const Metrics* o) : locator(nullptr) {
		current = o->current;
        locator =  std::make_unique<Locator>(o->locator.get(),this);
		currentTemplate = o->currentTemplate;
		segmentStack = o->segmentStack;
		nodeStack = o->nodeStack;
		page = o->page;
	}

	void Metrics::trace(Messages &errs,const mt::mstack* context) const {
		for (auto& i : *context) {
			if(i.first != nullptr) {
				errs << Message(Support::trace,i.first->name());
			}
		}
		if(currentTemplate != nullptr) {
			errs << Message(Support::trace,currentTemplate->name);
		} else {
			errs << Message(Support::trace,"direct"); //This is probably a test or a syntax check.
		}
	}

	const Node* Metrics::byPath(Messages &errs,const std::string &path,const mt::mstack* context) const {
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
			if (result == nullptr && errs.storing()) {
				errs.push(Message(error, "Path: " + path + " did not find a node."));
				trace(errs,context);
				errs.pop();
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
		if (result.first == nullptr && errs.storing()) {
			errs << Message(error, "Path: " + path + " did not find a node.");
		}
		if (result.second == UINTMAX_MAX) {
			errs << Message(range, "Page was out of bounds.");
		}
		locator->setRoot(Content::root());
		return result;
	}

}
