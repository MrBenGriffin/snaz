#include "Internals.h"
#include "InternalsCommon.h"
#include "support/Fandr.h"
#include "support/Convert.h"
#include "support/Message.h"

namespace mt {
	using namespace Support;

	void iLeft::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		string string_to_cut = my.parm(1);
		string chars_to_keep = my.parm(2);
		string result;
		pair<int, bool> int_res = integer(chars_to_keep);
		if (int_res.second) {
			if (!left(string_to_cut, int_res.first, result)) {
				e << Message({error,"iLeft found non-utf8 characters."});
			}
		} else {
			e << Message({error,"iLeft found a bad integer " + chars_to_keep + " in the second parameter."});
		}
		my.logic(result,2);
	}

	void iMid::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		string string_to_cut = my.parm(1);
		auto start = integer(my.parm(2));
		auto keep = integer(my.parm(3));
		string result;
		if(start.second && keep.second) {
			auto right_equiv = start.first > 0 ? 0 - start.first : INT_MAX;
			if (!right(string_to_cut,right_equiv,result)) {
				e << Message(error,"iMid found non-utf8 characters.");
			} else {
				if (!left(result,keep.first,result)) {
					e << Message(error,"iMid found non-utf8 characters.");
				} else {
					my.logic(result,3);
				}
			}
		} else {
			e << Message(error,"iMid found non number(s) in parameters 2/3.");
		}
	}

	void iRight::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		string string_to_cut = my.parm(1);
		string chars_to_keep = my.parm(2);
		string result;
		pair<int, bool> int_res = integer(chars_to_keep);
		if (int_res.second) {
			if (!right(string_to_cut, int_res.first, result)) {
				e << Message(error,"iRight found non-utf8 characters.");
			}
		} else {
			e << Message(error, "iRight found a bad integer " + chars_to_keep + " in the second parameter.");
		}
		my.logic(result,2);
	}
}
