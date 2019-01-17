#ifndef __MC_SCANNER_HPP__
#define __MC_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser.tab.hpp"
#include "location.hh"

namespace mt {

    class Advanced : public yyFlexLexer {
    public:
        bool stripped;
        bool defining;

        Advanced(std::istream *in,bool strip,bool define) : yyFlexLexer(in),stripped(strip),defining(define) {
        };

        virtual ~Advanced() {
        };

        using FlexLexer::yylex;
        virtual int yylex(mt::Parser::semantic_type *const lval,
                          mt::Parser::location_type *location);

    private:
         mt::Parser::semantic_type *yylval = nullptr;
    };

}

#endif
