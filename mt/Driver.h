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

#include "parser.tab.hpp"
#include "support/Message.h"

#include "mt/mt.h"
#include "mt/MacroText.h"

namespace mt {
	using namespace Support;

	class Driver {

	public:

		Driver(Messages&,std::istream &stream, bool);
		virtual ~Driver();

		void define(Messages&,parse_result&,bool);

		void new_macro( const std::string & );
		void storeWss(const std::string &);
		void store( const std::string &);
		void inject( const std::string &);
		void add_parm();
		void store_macro();
		void setPos(pos& p) {position = std::move(p);}

		void parse(Messages&,MacroText&,bool);
		static void parse(Messages&,MacroText&,const std::string&,bool = false);

	private:
		const std::istream*			source; //only used for parse errors..
		static int							accept;
		pos 										position;
		bool										iterated;

		MacroText*				_final;
		MacroText					parm;

		std::forward_list< std::unique_ptr<Macro> >	macro_stack;
//		std::forward_list<Macro>	macro_stack;

		Parser  *parser   = nullptr;
		Scanner *scanner  = nullptr;

		void parseError(Messages&);

	};

} /* end namespace mt */

#endif //DRIVER_H
