#ifndef __MC_SCANNER_HPP__
#define __MC_SCANNER_HPP__ 1

#include <cstdlib>
#include "parser.tab.hpp"
#include "location.hh"

namespace mt {
	class Scanner {
	public:
		bool stripped;
		bool defining;
		virtual int yylex(Parser::semantic_type *const, Parser::location_type *)=0;
		virtual ~Scanner() = default;
	};
}

#endif

