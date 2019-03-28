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
#include "support/Message.h"

namespace mt {
	using namespace Support;

	class Driver {

	public:

		Driver(Messages&,std::istream &stream, bool);

		virtual ~Driver();

		mtext parse(Messages&,bool);
		parse_result define(Messages&,bool);
		void expand(std::ostream&,Messages&,const std::string&);

		void new_macro( const std::string & );
		void storeWss(const std::string &);
		void store( const std::string &);
		void inject( const std::string &);
		void add_parm();
		void store_macro();
		void setPos(pos& p) {position = std::move(p);}

		static std::string expand(Messages&,std::string&,std::string); //syntactic sugar.
		static std::ostream& visit(const Token&, std::ostream&);
		static std::ostream& visit(const mtext&, std::ostream&);

		static void expand(const mtext&,Messages&,std::ostream&,mstack& = empty_stack);
		static void expand(const mtext&,Messages&,mtext&,mstack& = empty_stack);
		static void inject(const mtext&,Messages&,mtext&,mstack&);
		static void subs(const mtext&,mtext&,std::vector<std::string>&,const std::string&);


	private:
		const std::istream*			source; //only used for parse errors..
		static int					accept;
		pos 						position;
		static mstack				empty_stack;
		bool						iterated;
		mtext						final;
		mtext						parm;
		std::forward_list<Macro>	macro_stack;

		Parser  *parser   = nullptr;
		Scanner *scanner  = nullptr;

		void parseError(Messages&);

	};

} /* end namespace mt */

#endif //DRIVER_H
