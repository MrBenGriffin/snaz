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
#include <unordered_map>
#include <deque>
#include <functional>

#include "mt.h"
#include "advanced.h"
#include "parser.tab.hpp"

namespace mt {

	//This is the parser-injection.

	class Macro;

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
		static vMap   vis;
		static eMap   exp;

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
