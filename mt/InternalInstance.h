//
// Created by Ben on 2019-01-29.
//

#ifndef MACROTEXT_CURRENT_H
#define MACROTEXT_CURRENT_H

#include "mt.h"

namespace mt {
	class Internal;
	using namespace Support;
	// Used by Internals to hold current evaluation,
	// So using a base class isn't so good.
	// and also manages common utility functions.
	class InternalInstance {
	public:
		const Internal* owner;
		size_t          min{0}, max{0}, count {0};
		mtext*          output {nullptr};
		Instance*       instance {nullptr};
		const plist*    parms {nullptr};
		mstack*         context {nullptr};
		Messages*       errs;
		InternalInstance(const Internal*,Support::Messages&,mtext&,Instance&,mstack&);

		void generate(plist&,const mtext*,string,string);

		bool     		boolParm(size_t,bool=false); //a boolean value (with a default).
		std::string     parm(size_t);
		const mtext*    praw(size_t);
		void            expand(size_t);
		void            set(std::string);
		void            logic(bool,size_t);
		void            logic(size_t,size_t);
		void            logic(std::string&,size_t);
		void            logic(long double,std::string,size_t);

	};

}

#endif //MACROTEXT_CURRENT_H
