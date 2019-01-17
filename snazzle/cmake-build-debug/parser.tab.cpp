// A Bison parser, made by GNU Bison 3.2.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.





#include "parser.tab.hpp"


// Unqualified %code blocks.
#line 21 "parser.ypp" // lalr1.cc:438

   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include "../Driver.h"

#undef yylex
#define yylex scanner.yylex

#line 55 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:438


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 4 "parser.ypp" // lalr1.cc:513
namespace mt {
#line 150 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:513

  /// Build a parser object.
  Parser::Parser (Advanced  &scanner_yyarg, Driver    &driver_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      driver (driver_yyarg)
  {}

  Parser::~Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
    , location ()
  {}

#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value ()
    , location (std::move (that.location))
  {
    switch (this->type_get ())
    {
      case 3: // WS
      case 4: // TEXT
      case 5: // MACRO_IN
      case 6: // PARM
      case 7: // MACRO_OUT
      case 8: // INJECTION
        value.move< std::string > (std::move (that.value));
        break;

      default:
        break;
    }

  }
#endif

  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->type_get ())
    {
      case 3: // WS
      case 4: // TEXT
      case 5: // MACRO_IN
      case 6: // PARM
      case 7: // MACRO_OUT
      case 8: // INJECTION
        value.copy< std::string > (that.value);
        break;

      default:
        break;
    }

  }


  // Implementation of basic_symbol constructor for each type.
# if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, location_type&& l)
    : Base (t)
    , location (std::move (l))
  {}
#else
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , location (l)
  {}
#endif
# if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
    : Base (t)
    , value (std::move (v))
    , location (std::move (l))
  {}
#else
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}
#endif


  template <typename Base>
  Parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::clear ()
  {
    // User destructor.
    symbol_number_type yytype = this->type_get ();
    basic_symbol<Base>& yysym = *this;
    (void) yysym;
    switch (yytype)
    {
   default:
      break;
    }

    // Type destructor.
  switch (yytype)
    {
      case 3: // WS
      case 4: // TEXT
      case 5: // MACRO_IN
      case 6: // PARM
      case 7: // MACRO_OUT
      case 8: // INJECTION
        value.template destroy< std::string > ();
        break;

      default:
        break;
    }

    Base::clear ();
  }

  template <typename Base>
  bool
  Parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->type_get ())
    {
      case 3: // WS
      case 4: // TEXT
      case 5: // MACRO_IN
      case 6: // PARM
      case 7: // MACRO_OUT
      case 8: // INJECTION
        value.move< std::string > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_type.
  Parser::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  Parser::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  Parser::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  void
  Parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  void
  Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  int
  Parser::by_type::type_get () const
  {
    return type;
  }

  // Implementation of make_symbol for each symbol type.
# if 201103L <= YY_CPLUSPLUS
  inline
  Parser::symbol_type
  Parser::make_END (location_type l)
  {
    return symbol_type (token::END, std::move (l));
  }
#else
  inline
  Parser::symbol_type
  Parser::make_END (const location_type& l)
  {
    return symbol_type (token::END, l);
  }
#endif
# if 201103L <= YY_CPLUSPLUS
  inline
  Parser::symbol_type
  Parser::make_WS (std::string v, location_type l)
  {
    return symbol_type (token::WS, std::move (v), std::move (l));
  }
#else
  inline
  Parser::symbol_type
  Parser::make_WS (const std::string& v, const location_type& l)
  {
    return symbol_type (token::WS, v, l);
  }
#endif
# if 201103L <= YY_CPLUSPLUS
  inline
  Parser::symbol_type
  Parser::make_TEXT (std::string v, location_type l)
  {
    return symbol_type (token::TEXT, std::move (v), std::move (l));
  }
#else
  inline
  Parser::symbol_type
  Parser::make_TEXT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::TEXT, v, l);
  }
#endif
# if 201103L <= YY_CPLUSPLUS
  inline
  Parser::symbol_type
  Parser::make_MACRO_IN (std::string v, location_type l)
  {
    return symbol_type (token::MACRO_IN, std::move (v), std::move (l));
  }
#else
  inline
  Parser::symbol_type
  Parser::make_MACRO_IN (const std::string& v, const location_type& l)
  {
    return symbol_type (token::MACRO_IN, v, l);
  }
#endif
# if 201103L <= YY_CPLUSPLUS
  inline
  Parser::symbol_type
  Parser::make_PARM (std::string v, location_type l)
  {
    return symbol_type (token::PARM, std::move (v), std::move (l));
  }
#else
  inline
  Parser::symbol_type
  Parser::make_PARM (const std::string& v, const location_type& l)
  {
    return symbol_type (token::PARM, v, l);
  }
#endif
# if 201103L <= YY_CPLUSPLUS
  inline
  Parser::symbol_type
  Parser::make_MACRO_OUT (std::string v, location_type l)
  {
    return symbol_type (token::MACRO_OUT, std::move (v), std::move (l));
  }
#else
  inline
  Parser::symbol_type
  Parser::make_MACRO_OUT (const std::string& v, const location_type& l)
  {
    return symbol_type (token::MACRO_OUT, v, l);
  }
#endif
# if 201103L <= YY_CPLUSPLUS
  inline
  Parser::symbol_type
  Parser::make_INJECTION (std::string v, location_type l)
  {
    return symbol_type (token::INJECTION, std::move (v), std::move (l));
  }
#else
  inline
  Parser::symbol_type
  Parser::make_INJECTION (const std::string& v, const location_type& l)
  {
    return symbol_type (token::INJECTION, v, l);
  }
#endif


  // by_state.
  Parser::by_state::by_state ()
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  void
  Parser::by_state::clear ()
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  Parser::symbol_number_type
  Parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 3: // WS
      case 4: // TEXT
      case 5: // MACRO_IN
      case 6: // PARM
      case 7: // MACRO_OUT
      case 8: // INJECTION
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 3: // WS
      case 4: // TEXT
      case 5: // MACRO_IN
      case 6: // PARM
      case 7: // MACRO_OUT
      case 8: // INJECTION
        value.move< std::string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 3: // WS
      case 4: // TEXT
      case 5: // MACRO_IN
      case 6: // PARM
      case 7: // MACRO_OUT
      case 8: // INJECTION
        value.move< std::string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, YY_MOVE (yyla));
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 3: // WS
      case 4: // TEXT
      case 5: // MACRO_IN
      case 6: // PARM
      case 7: // MACRO_OUT
      case 8: // INJECTION
        yylhs.value.emplace< std::string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 6:
#line 54 "parser.ypp" // lalr1.cc:907
    { driver.store(yystack_[0].value.as< std::string > ()); }
#line 850 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 7:
#line 55 "parser.ypp" // lalr1.cc:907
    { driver.store(yystack_[0].value.as< std::string > ()); }
#line 856 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 8:
#line 56 "parser.ypp" // lalr1.cc:907
    { driver.inject(yystack_[0].value.as< std::string > ()); }
#line 862 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 9:
#line 57 "parser.ypp" // lalr1.cc:907
    { driver.store_macro(); }
#line 868 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 10:
#line 61 "parser.ypp" // lalr1.cc:907
    { driver.new_macro(yystack_[0].value.as< std::string > ()); }
#line 874 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 11:
#line 63 "parser.ypp" // lalr1.cc:907
    { driver.add_parm(yystack_[0].value.as< std::string > ()); }
#line 880 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 17:
#line 77 "parser.ypp" // lalr1.cc:907
    { driver.add_parm(yystack_[0].value.as< std::string > ()); }
#line 886 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 18:
#line 78 "parser.ypp" // lalr1.cc:907
    { driver.store(yystack_[0].value.as< std::string > ()); }
#line 892 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 19:
#line 79 "parser.ypp" // lalr1.cc:907
    { driver.store(yystack_[0].value.as< std::string > ()); }
#line 898 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 20:
#line 80 "parser.ypp" // lalr1.cc:907
    { driver.inject(yystack_[0].value.as< std::string > ()); }
#line 904 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;

  case 21:
#line 81 "parser.ypp" // lalr1.cc:907
    { driver.store_macro(); }
#line 910 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
    break;


#line 914 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:907
            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const signed char Parser::yypact_ninf_ = -13;

  const signed char Parser::yytable_ninf_ = -1;

  const signed char
  Parser::yypact_[] =
  {
       0,   -13,   -13,   -13,   -13,   -13,     1,     6,   -13,   -13,
      23,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,    17,
      23,   -13,   -13,    23,   -13
  };

  const unsigned char
  Parser::yydefact_[] =
  {
       0,     2,     7,     6,    10,     8,     0,     0,     4,     9,
      12,     1,     3,     5,    19,    18,    17,    20,    21,     0,
      13,    15,    11,    14,    16
  };

  const signed char
  Parser::yypgoto_[] =
  {
     -13,   -13,   -13,    -5,    12,   -13,   -13,   -12,    -7
  };

  const signed char
  Parser::yydefgoto_[] =
  {
      -1,     6,     7,     8,    18,    10,    19,    20,    21
  };

  const unsigned char
  Parser::yytable_[] =
  {
       1,    11,    13,     2,     3,     4,    12,    23,     5,     2,
       3,     4,     9,    24,     5,     0,    24,     0,     0,     9,
      14,    15,     4,    16,    22,    17,    14,    15,     4,    16,
       0,    17
  };

  const signed char
  Parser::yycheck_[] =
  {
       0,     0,     7,     3,     4,     5,     0,    19,     8,     3,
       4,     5,     0,    20,     8,    -1,    23,    -1,    -1,     7,
       3,     4,     5,     6,     7,     8,     3,     4,     5,     6,
      -1,     8
  };

  const unsigned char
  Parser::yystos_[] =
  {
       0,     0,     3,     4,     5,     8,    10,    11,    12,    13,
      14,     0,     0,    12,     3,     4,     6,     8,    13,    15,
      16,    17,     7,    16,    17
  };

  const unsigned char
  Parser::yyr1_[] =
  {
       0,     9,    10,    10,    11,    11,    12,    12,    12,    12,
      14,    13,    15,    15,    15,    16,    16,    17,    17,    17,
      17,    17
  };

  const unsigned char
  Parser::yyr2_[] =
  {
       0,     2,     1,     2,     1,     2,     1,     1,     1,     1,
       0,     4,     0,     1,     2,     1,     2,     1,     1,     1,
       1,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "WS", "TEXT", "MACRO_IN",
  "PARM", "MACRO_OUT", "INJECTION", "$accept", "list_option", "list",
  "item", "macro", "$@1", "parms", "parm", "pitem", YY_NULLPTR
  };


  const unsigned char
  Parser::yyrline_[] =
  {
       0,    46,    46,    46,    49,    50,    54,    55,    56,    57,
      61,    61,    67,    68,    69,    73,    74,    77,    78,    79,
      80,    81
  };

  // Print the state stack on the debug stream.
  void
  Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  Parser::token_number_type
  Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8
    };
    const unsigned user_token_number_max_ = 263;
    const token_number_type undef_token_ = 2;

    if (static_cast<int> (t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 4 "parser.ypp" // lalr1.cc:1218
} // mt
#line 1255 "/Users/ben/CLionProjects/snazzle/cmake-build-debug/parser.tab.cpp" // lalr1.cc:1218
#line 84 "parser.ypp" // lalr1.cc:1219

void mt::Parser::error( const location_type &l, const std::string &err_message ) {
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
