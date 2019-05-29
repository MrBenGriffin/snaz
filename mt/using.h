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
	/**
	 * All macro-text is made up of tokens.
	 * Tokens are one of (macro,script,injection), and script is one of (text,wss).
	 * Each token, being an object, may be best handled as a reference (or ptr).
	 * Macro Tokens refer to Handlers, which are in two flavours: Definitions, and Internals.
	 * Both flavours have a name (string) and may receive parameters, while 'internals' have c++ handling of parameters,
	 * definitions expand macro-text.
	 *
	 * When a macro is evaluated a Carriage is invoked, which consists of
	 *  (1) the Handler and
	 *  (2) the Instance -
	 *   (a) the macro-token's parameters (as macro-text),
	 *   (b) the current context - the stack of current Carriages.
	 *   (c) the current metrics - current node details.
	 * the current macro stack, and a metrics object (which contains current node details).
	 * Carriage will return the evaluation as macro-text.
	 *
	 * Chunks of macrotext are passed around (and copied) a lot.
	 * For example, in a for..to loop, the evaluation section will be reprocessed on each iteration.
	 * Iterations are managed by both 'forStuff' objects and iteration objects.
	 * forStuff is for substitution of special text tokens, while iterations manage repeated loops.
	 *
	 * Tokens themselves may be merged (if they are script tokens) as they are added.
	 * So it may be good to declare a proper MacroText class which can handle all this (rather than use static Token).
	 *
	 * Actions.
	 * (1) On load / initiate the mt system, we be hold all Handlers as unique_ptr and
	 * returning const Handler* because they should never be changed by anything else.
	 *
	 * (2) Add a MacroText class for token management, and use 'add' or << methods for token management.
	 *
	 *
	 **/
	class Token;
	class Instance;
	class forStuff;    //inside Instance.h
	class Handler;
	using TokenPtr=     std::shared_ptr<Token>;
	using mtext=		std::deque<TokenPtr>;
	using parse_result=	std::pair<bool,std::pair<mtext, bool>>;
	using iteration=	std::pair<size_t,size_t>;
	using plist=		std::vector<mtext>;
	using nlist=		std::vector<const node::Node *>;
	using pos=			std::pair<size_t,size_t>;
	//Instances need to be shared, because of subcontexts in injections.
	using Carriage=		std::pair<const Handler*, std::shared_ptr<Instance> >;
	using mstack=		std::deque< Carriage >;

}

#endif //MACROTEXT_USING_H
