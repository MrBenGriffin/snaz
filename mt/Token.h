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
	class Script;
	class Token {
	public:
		Token() = default;
		virtual ~Token() = default;

		//virtual
		virtual void expand(Messages&,mtext&,mstack&) const;
		virtual void doFor(mtext&,const forStuff&) const;
		virtual bool empty() const;

		//pure virtual
		virtual std::string name() const = 0;
		virtual std::string get() const =0;             	 //return text final rendered.
		virtual std::ostream& visit(std::ostream&) const =0; //return text version of token.
		virtual void final(std::ostream&) const=0;           //return text final rendered.
		virtual void inject(Messages&,mtext&,mstack&) const = 0;
		virtual void subs(mtext&,const std::vector<std::string>&,const std::string&) const =0;

		// My function!!
		static void add(TokenPtr&,mtext&);
		static void add(std::string,mtext&);
		static void add(std::ostream&,mtext&);
		static void add(const Script*,mtext&);

	};
}


#endif //MACROTEXT_TOKEN_H
