//
// Created by Ben Griffin on 2019-02-07.
//

#ifndef MACROTEXT_INSTANCE_H
#define MACROTEXT_INSTANCE_H

#include <string>
#include <variant>
#include <ostream>
#include <vector>
#include <deque>

#include "declarations.h"

namespace mt {
	using mtext=std::deque<Token>;
	using parse_result=std::pair<mtext, bool>;
	using iteration=std::pair<size_t,size_t>;
	using plist=std::vector<mtext>;

	class Instance {
	public:
		const plist *parms = nullptr;
		iteration it = {0, 0};
		bool generated;     //internal generation via e.g. iForX
		std::string iValue; //substitutes for iForX
		std::string iCount; //substitutes for iForX
		Instance(const plist *, iteration, bool= false);
		Instance(const Instance &);
	};

	using Carriage= std::pair<Handler*, Instance>;
	using mstack=std::deque< Carriage >;

}


#endif //MACROTEXT_INSTANCE_H
