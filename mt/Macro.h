//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MACRO_H
#define MACROTEXT_MACRO_H

#include <string>
#include "support/Message.h"
#include "Token.h"
#include "mt/using.h"

namespace mt {
	using namespace Support;
	/**
		* an instance of this is a macro token instance in a piece of text.
		* It may be PART of a usermacro definition.
		* With a context, it can be evaluete
	**/
	class Macro : public Token {
	public:
		std::string _name;
		plist parms;        /* parsed parms */

		std::string get() const override;             //return text.
		bool empty() const override;
		void final(std::ostream&) const override;     //return final text.
		std::ostream& visit(std::ostream&) const override;
		void inject(Messages&,mtext&,mstack&) const override;
		void doFor(mtext&,const forStuff&) const override;
		void subs(mtext&,const std::vector<std::string>&,const std::string&) const override;
		void expand(Messages&,mtext&,mstack&) const override;
		std::string name() const override { return _name; }
		void add(mtext&) override;

		//used for expanding injections, etc. not sure if this should generate a copy or not..

		explicit Macro(std::string);
		~Macro() override = default;
	};
}

#endif //MACROTEXT_MACRO_H
