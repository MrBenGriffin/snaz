//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MACRO_H
#define MACROTEXT_MACRO_H

#include <string>
#include "mt.h"


namespace mt {
	using namespace Support;
	/**
		* an instance of this is a macro token instance in a piece of text.
		* It may be PART of a usermacro definition.
		* With a context, it can be evaluete
	**/
	class Macro {
	public:
		std::string name;
		plist parms;        /* parsed parms */
		void expand(Messages&,mtext&,mstack&) const;
		std::ostream& visit(std::ostream&) const;

		//used for expanding injections, etc. not sure if this should generate a copy or not..
		void inject(Messages&,mtext&,mstack&) const;
		void subs(mtext&,std::vector<std::string>& ,const std::string&) const;
		void doFor(mtext&,const forStuff&) const;

		explicit Macro(std::string);
		void add(mtext&);
	};
}

#endif //MACROTEXT_MACRO_H
