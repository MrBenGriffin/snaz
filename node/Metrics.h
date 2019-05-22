//
// Created by Ben on 2019-04-24.
//

#ifndef MACROTEXT_METRICS_H
#define MACROTEXT_METRICS_H

#include <stack>
#include <deque>
#include "support/Message.h"

namespace content {
	class Template;
	class Segment;
}
namespace node {
	class Content;
	class Locator;
	class Node;

	using namespace Support;

	class Metrics {
		/*
		 * A set of values which record current state during a single node's build.
		 * By capturing them here we can use future/promises for node building (one day).
		 *
		 * wss breaks should be here too.. and any build-related 'statics'
		 */
	public:
		Metrics();
		~Metrics();
		Metrics(const Metrics*);
        std::shared_ptr<Locator> locator;

		const Content *current;
		const content::Template *currentTemplate;
		std::stack<const content::Segment *> segmentStack;
		std::deque<const Content *> nodeStack;    //current node - used to pass to built-in functions
		size_t page;
 		void setLocator(std::shared_ptr<Locator> l) { locator = l; }
		const Node* byPath(Messages &,const std::string &) const;    				//returns null if not found
		pair<const Node*,size_t> nodePage(Messages &,const std::string &) const;    //returns null if not found
		void push(const Content*,const content::Segment*);
		void pop();

	};
}

#endif //MACROTEXT_METRICS_H
