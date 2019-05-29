//
// Created by Ben on 2019-05-01.
//

#ifndef MACROTEXT_TOKEN_H
#define MACROTEXT_TOKEN_H

#include <string>
#include <vector>
#include "support/Message.h"
#include "mt/using.h"


using namespace Support;

namespace mt {
	class Token {
	public:
		Token() = default;
		virtual ~Token() = default;

		//virtual
		virtual void expand(Messages&,MacroText&,mstack&) const;
		virtual void doFor(MacroText&,const forStuff&) const;
		virtual bool empty() const;

		//pure virtual
		virtual std::string name() const = 0;
		virtual std::string get() const =0;             	 //return text final rendered.
		virtual std::ostream& visit(std::ostream&) const =0; //return text version of token.
		virtual void final(std::ostream&) const=0;           //return text final rendered.
		virtual void inject(Messages&,MacroText&,mstack&) const = 0;
		virtual unique_ptr<Token> clone() const = 0;
		virtual void subs(const std::vector<std::string>&,const std::string&) =0;

	};
}


#endif //MACROTEXT_TOKEN_H
