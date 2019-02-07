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
#include <memory>

#include "mt.h"
#include "parser.tab.hpp"

namespace mt {
	using namespace Support;
	class Driver {

	public:

		Driver(std::istream &stream, bool);

		virtual ~Driver();

		mtext parse(bool);
		parse_result define(bool);
		void expand(std::ostream&,Messages&,const std::string &);

		void new_macro( const std::string & );
		void storeWss(const std::string &);
		void store( const std::string &);
		void inject( const std::string &);
		void add_parm();
		void store_macro();

		static std::ostream& visit(const Token&, std::ostream&);
		static std::ostream& visit(const mtext&, std::ostream&);
		static void expand(const mtext&,Messages&,std::ostream&,mstack&);
		static void expand(const mtext&,Messages&,mtext&,mstack& = empty_stack);
		static void inject(const mtext&,Messages&,mtext&,mstack&);
		static void subs(const mtext&,mtext&,std::vector<std::string>&,const std::string&);


	private:
		static int					accept;
		static mstack				empty_stack;
		bool						iterated;
		mtext						final;
		mtext						parm;
		std::forward_list<Macro>	macro_stack;

//		void parse_helper( std::istream &stream, bool, bool, bool );
		Parser  *parser   = nullptr;
		Scanner *scanner  = nullptr;

	};

} /* end namespace mt */

#endif //DRIVER_H
