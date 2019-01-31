//
// Created by Ben Griffin on 2019-01-31.
//

#include <iostream>

#include "Internals.h"
#include "InternalsCommon.h"
#include "support/Infix.h"
#include "support/Message.h"

namespace mt {
	using namespace Support;

	void iExists::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		string key = my.parm(1);
		auto sr = storage.find(key);
		my.logic(sr.found,2); //@iExists(A)=1
	}

	void iGet::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		string key = my.parm(1);
		auto sr = storage.find(key);
		if(sr.found) {
			my.logic(sr.result,1); //@iGet(A,B)=2
		}
	}

	void iSet::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		string key = my.parm(1);
		string value;
		if(my.count == 2) {
			value = my.parm(2);
		}
		storage.set(key,value);
	}

}