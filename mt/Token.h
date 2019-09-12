//
// Created by Ben on 2019-05-01.
//

#ifndef MACROTEXT_TOKEN_H
#define MACROTEXT_TOKEN_H

#include <string>
#include <deque>
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
		virtual bool empty() const;
		virtual void doFor(Messages&,MacroText&,const forStuff&, mstack&) const;

		//pure virtual
//		virtual bool empty() const;
		virtual std::string name() const = 0;
		virtual std::string get() const =0;             	 //return text final rendered.
		virtual std::ostream& visit(std::ostream&) const =0; //return text version of token.
		virtual void final(std::ostream&) const=0;           //return text final rendered.
		virtual void inject(Messages&,MacroText&,mstack&) const = 0;

		virtual void clone(MacroText&) const = 0;
		virtual void subs(MacroText&,const std::deque<std::string>&,const std::string&) const =0;

	};
}


#endif //MACROTEXT_TOKEN_H
