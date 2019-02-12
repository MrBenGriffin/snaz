#ifndef __MC_SCANNER_HPP__
#define __MC_SCANNER_HPP__ 1

#include <cstdlib>
#include "parser.tab.hpp"
#include "location.hh"
#include "support/Message.h"

namespace mt {
	class Scanner {
	protected:
		Support::Messages& errs;
	public:
		Scanner(Support::Messages& m) : errs(m) {}
		bool stripped;
		bool defining;
		virtual int yylex(Parser::semantic_type *const, Parser::location_type *)=0;
		virtual void LexerError(const char*) {};
		virtual ~Scanner() = default;
	};
}

#endif

