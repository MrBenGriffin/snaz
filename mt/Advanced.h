//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_ADVANCED_H
#define MACROTEXT_ADVANCED_H

#undef yyFlexLexer
#define yyFlexLexer aFlexLexer
#include <FlexLexer.h>
#include "Scanner.h"
#include "parser.tab.hpp"
#include "location.hh"

namespace mt {
	class Advanced : public Scanner, public aFlexLexer {
	public:
		Advanced(std::istream *in) : Scanner(),aFlexLexer(in) {}
		virtual ~Advanced() = default;
		using aFlexLexer::yylex;
		virtual int yylex(mt::Parser::semantic_type *const,mt::Parser::location_type *);
	private:
		mt::Parser::semantic_type *yylval = nullptr;
	};
}

#endif //MACROTEXT_ADVANCED_H
