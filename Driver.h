//
// Created by Ben on 2019-01-09.
//

#ifndef MT_DRIVER_H
#define MT_DRIVER_H

#include <any>
#include <type_traits>
#include <string>
#include <cstddef>
#include <istream>
#include <sstream>
#include <vector>
#include <forward_list>
#include <deque>

#include "advanced.h"
#include "parser.tab.hpp"
//#include "userMacro.h"

class userMacro;

namespace mt {

    using mtext=std::deque<std::any>;
    using parse_result=std::pair<mtext,bool>;
    using plist=std::vector<mtext>;
    using mstack=std::deque< std::pair< const userMacro *,plist>>;

    //This is the parser-injection.
    class Injection {
    private:
        enum class It { plain,text,current,count,size }; //not iteration,not injection,i,j,k,n respectively
        It type;            // tells us the iteration type
        size_t value;       // the parameter number being asked for (when plain)
        size_t sValue;      // the stack offset
        long offset;        // the offset value (signed!)
        bool modulus;       // if the offset is a modulus.
        bool stack;         // if this is a full stack trace
        bool list;          // if we are passing a list of parameters over (eg %(3+))
        std::string basis;  // What we are working with.

        void parseStart();
        void parseIterated();
        void parsePName();
        void parseBrackets();
        void parseParent();
    public:
        bool iterator;      // This is injection defines the macro as an iterator.
        Injection(const std::string);
        std::ostream& visit(std::ostream&);
        void expand(std::ostream&,mstack&);

    };

	class wss {
	public:
		std::string text;
//		friend std::string* (*std::any_cast<std::string>(wss& o)) { return o.text; }
//		operator std::string() { return text; }
		wss(const std::string &);
		wss(wss&,const std::string &);
	};

	/**
     * This is the parser-macro.
     * It forms a part of the macrotext.
     * It needs context to be an instance.
     * It may be PART of a usermacro definition.
    **/
    class macro {
      public:
        std::string name;
        plist parms;
        void expand(std::ostream&,mstack&);
        std::ostream& visit(std::ostream&);
        explicit macro(std::string);
        macro(const macro &);
     };

    class Driver {
    public:
		static size_t mac_type;
		static size_t inj_type;
		static size_t str_type;
		static size_t wss_type;
		Driver();

        virtual ~Driver();

        mtext parse(std::istream &iss,bool,bool);
        parse_result define(std::istream &iss,bool,bool);

        void new_macro( const std::string & );
		void storeWss(const std::string &);
        void store( const std::string &);
        void inject( const std::string &);
        void add_parm( /*const std::string &*/ );
        void store_macro();

		static std::ostream& visit(std::any&, std::ostream &stream);
        static std::ostream& visit(mtext&, std::ostream &stream);
        static void expand(mtext&,std::ostream&,mstack& = empty_stack);

	private:
		static mstack empty_stack;

        bool         iterated;

        mtext        final;
        mtext        parm;
        std::forward_list<macro>    macro_stack;

        void parse_helper( std::istream &stream, bool, bool, bool );

        Parser  *parser  = nullptr;
        Advanced *scanner = nullptr;

    };

} /* end namespace mt */

#endif //DRIVER_H
