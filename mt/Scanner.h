#ifndef __MC_SCANNER_HPP__
#define __MC_SCANNER_HPP__ 1

#include <cstdlib>
#include <stack>
#include "parser.tab.hpp"
#include "location.hh"
#include "support/Message.h"

namespace mt {
	class Scanner {
	protected:
		Support::Messages& errs;
		std::stack<Parser::location_type> lit_stack;
		std::stack<Parser::location_type> mac_stack;
		std::stack<Parser::location_type> brc_stack;
		std::stack<Parser::location_type> brk_stack;
	public:
		Scanner(Support::Messages& m) : errs(m) {}
		bool stripped;
		bool defining;
		virtual int yylex(Parser::semantic_type *const, Parser::location_type *)=0;
		virtual void LexerError(const char* error) {
			std::string text(error);
			errs.add(Support::Message(Support::syntax, text));
		};
		virtual ~Scanner() = default;
	};
}

#endif

