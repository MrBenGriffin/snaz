//
// Created by Ben on 2019-01-23.
//

#ifndef MACROTEXT_MACRO_H
#define MACROTEXT_MACRO_H

#include <memory>
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
	class MacroText;
	class Wss;
	class Text;
	class Injection;
	class Macro : public Token {

	public:
		std::string _name;
		plist parms;        /* parsed parms */

		void addParms(size_t = 1);
		void addParm(unique_ptr<Wss>);
		void addParm(unique_ptr<Text>);
		void addParm(unique_ptr<Macro>);
		void addParm(unique_ptr<Injection>);

		std::string get() const override;             //return text.
		bool empty() const override;
		void final(std::ostream&) const override;     //return final text.
		std::ostream& visit(std::ostream&) const override;
		void inject(Messages&,MacroText&,mstack&) const override;
		void doFor(Messages&,MacroText&,const forStuff&,mstack&) const override;
		void subs(MacroText&,const std::deque<std::string>&,const std::string&) const override;
		void expand(Messages&,MacroText&,mstack&) const override;
		std::string name() const override { return _name; }
		void clone(MacroText&) const override;

		explicit Macro(std::string);
		explicit Macro(const Macro*);
		Macro(std::string,plist);
		~Macro() override = default;
	};
}

#endif //MACROTEXT_MACRO_H
