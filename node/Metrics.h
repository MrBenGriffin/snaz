//
// Created by Ben on 2019-04-24.
//

#ifndef MACROTEXT_METRICS_H
#define MACROTEXT_METRICS_H

#include <stack>
#include <deque>
#include <memory>
#include "support/Message.h"

namespace mt {
	class Handler;
	class Instance;
	using Carriage= std::pair<const Handler*, Instance* >;
	using mstack  = std::deque<Carriage>;
}
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
        std::unique_ptr<Locator> locator;

		const Content *current;
		const content::Template *currentTemplate;
		std::stack<bool> nullStack;
		std::stack<const content::Segment *> segmentStack;
		std::deque<const Content *> nodeStack;    //current node - used to pass to built-in functions
		size_t page;
		const Node* byPath(Messages &,const std::string &,const mt::mstack*) const;    	//returns null if not found
		pair<const Node*,size_t> nodePage(Messages &,const std::string &) const;    //returns null if not found
		void push(const Content*,const content::Segment*);
		void pop();
		void trace(Messages&,const mt::mstack*) const;

	};
}

#endif //MACROTEXT_METRICS_H
