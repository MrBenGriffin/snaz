//
// Created by Ben on 2019-01-09.
//

#ifndef MT_DRIVER_H
#define MT_DRIVER_H

#include <variant>
#include <type_traits>
#include <string>
#include <cstddef>
#include <istream>
#include <sstream>
#include <vector>
#include <forward_list>
#include <unordered_map>
#include <deque>
#include <functional>

#include "mt.h"
#include "advanced.h"
#include "parser.tab.hpp"

namespace mt {

	//This is the parser-injection.

    class Driver {

	public:

		Driver() = default;

        virtual ~Driver();

        mtext parse(std::istream &iss,bool,bool);
        parse_result define(std::istream &iss,bool,bool);

        void new_macro( const std::string & );
		void storeWss(const std::string &);
        void store( const std::string &);
        void inject( const std::string &);
        void add_parm();
        void store_macro();

		static std::ostream& visit(const Token&, std::ostream &stream);
        static std::ostream& visit(const mtext&, std::ostream &stream) ;
        static void expand(const mtext&,std::ostream&,mstack& = empty_stack);

	private:
		static 		 mstack empty_stack;
        bool         iterated;
        mtext        final;
        mtext        parm;
        std::forward_list<Macro>    macro_stack;

        void parse_helper( std::istream &stream, bool, bool, bool );

        Parser  *parser  = nullptr;
        Advanced *scanner = nullptr;

    };

} /* end namespace mt */

#endif //DRIVER_H
