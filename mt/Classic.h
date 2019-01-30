//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_CLASSIC_H
#define MACROTEXT_CLASSIC_H

#undef yyFlexLexer
#define yyFlexLexer cFlexLexer
#include <FlexLexer.h>
#include "Scanner.h"
#include "parser.tab.hpp"
#include "location.hh"

namespace mt {
	class Classic : public Scanner, public cFlexLexer {
	public:
		Classic(std::istream *in) : Scanner(),cFlexLexer(in) {}
		virtual ~Classic() = default;
		using cFlexLexer::yylex;
		virtual int yylex(mt::Parser::semantic_type *const,mt::Parser::location_type *);
	private:
		mt::Parser::semantic_type *yylval = nullptr;
	};
}

#endif //MACROTEXT_CLASSIC_H
