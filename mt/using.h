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
	 * √ On load / initiate the mt system, we be hold all Handlers as unique_ptr and
	 *   returning const Handler* (if necessary) because they should never be changed by anything else.
	 *
	 * √ Instances should be unique to their context, and referenced via a const*, therefore they do
	 *   not need to be managed using smart pointers.  As, however, they may be amended by the evaluator,
	 *   and pass by reference has a cost, it may be better just to use a * (eep!!) - but NOT to store it.
	 *
	 * (2) Add a MacroText class for token management, and use 'add' or << methods for token management.
	 *
	 * Parameter rules: Pass by...
	 * (P1) .. value when the function does not want to modify the parameter and the value is easy to copy (ints, doubles, char, bool, etc... simple types. std::string, std::vector, and all other STL containers are NOT simple types.)
	 * (P2) .. const pointer when the value is expensive to copy AND the function does not want to modify the value pointed to AND NULL is a valid, expected value that the function handles.
	 * (P3) .. non-const pointer when the value is expensive to copy AND the function wants to modify the value pointed to AND NULL is a valid, expected value that the function handles.
	 * (P4) .. const reference when the value is expensive to copy AND the function does not want to modify the value referred to AND NULL would not be a valid value if a pointer was used instead.
	 * (P5) .. non-const reference when the value is expensive to copy AND the function wants to modify the value referred to AND NULL would not be a valid value if a pointer was used instead.	 *
	 *
	 *
	 **/
	class Token;
	class Instance;
	class forStuff;    //inside Instance.h
	class Handler;
	class MacroText;
//	using MacroText=		MacroText;
	using parse_result=	std::pair<bool,std::pair<MacroText, bool>>;
	using iteration=	std::pair<size_t,size_t>;
	using plist=		std::vector<MacroText>;
	using nlist=		std::vector<const node::Node *>;
	using pos=			std::pair<size_t,size_t>;
	//Instances need to be shared, because of subcontexts in injections.
	using Carriage=		std::pair<const Handler*, Instance* >;
	using mstack=		std::deque< Carriage >;

}

#endif //MACROTEXT_USING_H
