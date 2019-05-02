//
// Created by Ben on 2019-05-02.
//

#ifndef MACROTEXT_USING_H
#define MACROTEXT_USING_H

#include <deque>
#include <vector>

namespace node {
	class Node;
}

namespace mt {
	class Token;
	class Instance;
	class forStuff;    //inside Instance.h
	class Handler;
	using mtext=		std::deque<std::shared_ptr<Token>>;
	using parse_result=	std::pair<bool,std::pair<mtext, bool>>;
	using iteration=	std::pair<size_t,size_t>;
	using plist=		std::vector<mtext>;
	using nlist=		std::vector<const node::Node *>;
	using pos=			std::pair<size_t,size_t>;
	using Carriage=		std::pair<const Handler*, Instance>;
	using mstack=		std::deque< Carriage >;

}

#endif //MACROTEXT_USING_H
