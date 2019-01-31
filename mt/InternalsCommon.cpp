//
// Created by Ben on 2019-01-29.
//

#include "InternalsCommon.h"
#include "Internals.h"

namespace mt {

	InternalsCommon::InternalsCommon(const Internal *thing,Messages& e,mtext &o, Instance &i, mstack &c) :
			output(&o), instance(&i), context(&c),errs(&e) {
		parms = i.parms;
		count = parms->size();
		min = thing->minParms;
		max = thing->maxParms;
	}

	std::string InternalsCommon::parm(size_t i) {
		std::ostringstream result;
		Driver::expand((*parms)[i - 1],*errs, result, *context);
		return result.str();
	}

	const mtext*	InternalsCommon::praw(size_t i) {
		const mtext* m = nullptr;
		if(parms->size() >= i) {
			m = &((*parms)[i - 1]);
		}
		return m;
	}


	void InternalsCommon::expand(size_t i) {
		Driver::expand((*parms)[i - 1],*errs, *output, *context);
	}

	void InternalsCommon::set(std::string str) {
		output->emplace_back(Text(str));
	}

	//offset points at the position of the TRUE parm.
	void InternalsCommon::logic(bool equals,size_t offset) {
		if(count >= offset) {
			if (equals) {
				expand(offset);
			} else {
				if (count > offset) {
					expand(offset+1);
				} //else return empty-string.
			}
		} else {
			set(equals ? "1" : "0");
		}
	}

	//this defaults to outputting basis, unless offset is smaller than count..
	//offset: 1-indexed parm (where basis was)
	//eg iLayout(I0,Foo,T,F). =>> logic(Foo,2)
	void InternalsCommon::logic(std::string& left,size_t offset) {
		if(count == offset) {
			set(left);
		} else {
			std::string right = parm(offset+1);
			logic(left == right,offset+2);
		}
	}
}