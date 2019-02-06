#include "Internals.h"
#include "InternalsCommon.h"
#include "support/Fandr.h"
#include "support/Convert.h"
#include "support/Message.h"
#include "support/Regex.h"

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
		my.logic(result,3); // @iLeft(text,term,?,T,F)
	}

	void iLength::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		size_t value=0;
		if(Support::length(my.parm(1),value)) {
			my.logic(value,2); // @iLength(text,?,T,F)
		} else {
			e << Message(error,"iLength found non-utf8 characters.");
		}
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
					my.logic(result,4); // @iMid(text,l,r,?,T,F)
				}
			}
		} else {
			e << Message(error,"iMid found non number(s) in parameters 2/3.");
		}
	}

	void iPosition::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		size_t value=0;
		if(Support::position(my.parm(2),my.parm(1),value)) {
			my.logic(value+1,3); //iPosition(foo,o,?,T,F)
		} else {
			my.logic(value,3);   //iPosition(foo,o,?,T,F)
		}
	}

	void iRegex::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		if(Regex::available(e)) {
/*
			InternalsCommon my(this,e,o,instance,context);
			if(my.count > 2) { //otherwise there's nothing to do.
				string pattern = my.parm(1);
				string substitute = my.praw(2);
				string scope = my.parm(3);
				string result;
				if (pattern.length() > 0 && scope.length() > 0) {
//					String::Regex::replace(pattern,substitute,scope);
//					if(!scope.empty()) {
//						process(&output,titleName,scope,0,true);
//					}
				}
				my.logic(result,4); //@iRegex(o,p,foo,fpp?,T,F)
			}
*/
		} else {
			e << Message(fatal,"iRegex requires the pcre library which was not found.");
		}
	}
	void iRembr::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		string result = my.parm(1);
		tabstrip(result); 	//should we strip out crlf also?!
		fandr(result,"\n"); //Should this be here?!
		my.set(result);
	}
	void iRembrp::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		my.set(my.parm(1));
	}
	void iReplace::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		if(my.count > 2) { //otherwise there's nothing to do. It's legal, but nothing to do this time.
			string search = my.parm(1);
			string replace = my.parm(2);
			string result = my.parm(3);
			if (search.length() > 0 && result.length() > 0)  {
				fandr(result,search,replace);
			}
			my.logic(result,4); //@iReplace(o,p,foo,fpp?,T,F)
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
		my.logic(result,3); // @iRight(text,term,?,T,F)
	}

	void iTrim::expand(Messages& e,mtext& o,Instance& instance,mstack& context) {
		InternalsCommon my(this,e,o,instance,context);
		string stringToTrim = my.parm(1);
		Support::trim(stringToTrim);
		my.set(stringToTrim);
	}

}
