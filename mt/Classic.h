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
#include "support/Message.h"

namespace mt {
	class Classic : public Scanner, public cFlexLexer {
	public:
		Classic(Support::Messages& e,std::istream *in) : Scanner(e),cFlexLexer(in) {}
		virtual ~Classic() = default;
		using cFlexLexer::yylex;
		virtual int yylex(mt::Parser::semantic_type *const,mt::Parser::location_type *);
		virtual void LexerError(const char* msg);
	private:
		mt::Parser::semantic_type *yylval = nullptr;
	};
}

#endif //MACROTEXT_CLASSIC_H
