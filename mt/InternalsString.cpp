#include "Internal.h"
#include "InternalInstance.h"
#include "support/Fandr.h"
#include "support/Convert.h"
#include "support/Message.h"
#include "support/Regex.h"

namespace mt {
	using namespace Support;

	void iLeft::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
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

	void iLength::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		size_t value=0;
		if(Support::length(my.parm(1),value)) {
			my.logic(value,2); // @iLength(text,?,T,F)
		} else {
			e << Message(error,"iLength found non-utf8 characters.");
		}
	}

	void iMid::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
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

	void iPosition::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		size_t value=0;
		if(Support::position(my.parm(2),my.parm(1),value)) {
			my.logic(value+1,3); //iPosition(foo,o,?,T,F)
		} else {
			my.logic(value,3);   //iPosition(foo,o,?,T,F)
		}
	}

	void iRegex::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		if(Regex::available(e)) {
			InternalInstance my(this,e,o,instance,context);
			if(my.count > 2) { //otherwise there's nothing to do.
				/**
				 * Pattern	(p1) :easy. It's going to be the regex pattern to search for.
				 * Scope	(p3) :could be easy if we put evaluation into the substitute.
				 * Subst 	(p2) :not easy. we need to look for \1, \2 etc. for substitution points.
				 */
				string final;
				string scope = my.parm(3);
				if (!scope.empty()) {
					int ovc = 30;                // size of ovector
					int *ov = nullptr;            // size must be a multiple of 3.
					ov = new int[ovc];            // 30 gives us 10 substitutions \1 etc..
					size_t count = 0;
					size_t start = 0;
					size_t lastEnd = string::npos;
					const mtext *subst = my.praw(2);
					string pattern = my.parm(1);
					size_t base_len = scope.length();
					mtext result;
					while (start <= base_len) {
						int matches = Regex::matcher(e, pattern, scope, (int) start, ov, ovc);
						if (matches <= 0) {
							final = scope;
							break;    //we are finished - either with nothing, or all done.
						}
						size_t matchstart = (size_t) ov[0], matchend = (size_t) ov[1];
						if (matchstart == matchend && matchstart == lastEnd) {
							matchend = start + 1;
							if (start + 1 < base_len && scope[start] == '\r' && scope[start + 1] == '\n') {
								matchend++;
							}
							while (matchend < base_len && (scope[matchend] & 0xc0) == 0x80) {
								matchend++;
							}
							if (matchend <= base_len) {
								Token::add(string(scope, start, matchend - start),result);
							}
							start = matchend;
						} else {
							Token::add(string(scope, start, matchstart - start),result);
							vector<string> subs;
							for (size_t n = 0; n < 10; n++) {
								string substr;
								int pos = ov[2 * n],pos2 = ov[2 * n + 1];
								if (pos >= scope.size()) break;
								if (pos >= 0) {
									if(pos2 >= scope.length()) {
										substr = scope.substr(pos);
									} else {
										substr = scope.substr(pos,pos2-pos);
									}
								}
								subs.push_back(substr);
							}
//							we have a praw (substitute).
//							we need to use the subs as injection points on the texts within it.
//							Regex substitutes are \1 \2 etc..
							Driver::subs(*subst,result,subs,"\\");
							start = matchend;
							lastEnd = matchend;
							count++;
						}
						count++;
					}
					if (count != 0 && start < base_len) {
						Token::add(string(scope, start, base_len - start),result);
					}
					delete[] ov;
					if(!result.empty()) {
						std::ostringstream expr;
						Driver::expand(e, result, expr, context);
						final = expr.str();
					}
				}
				my.logic(final,4); //@iRegex(o,p,foo,fpp?,T,F)
			} //count of 2 or less.
		} else {
			e << Message(fatal,"iRegex requires the pcre library which was not found.");
		}
	}

	void iRembr::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		string result = my.parm(1);
		tabstrip(result); 	//should we strip out crlf also?!
		fandr(result,"\n"); //Should this be here?!
		my.set(result);
	}
	void iRembrp::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		my.set(my.parm(1));
	}
	void iReplace::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
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

	void iRight::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
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

	void iTrim::expand(Messages& e,mtext& o,Instance& instance,mstack& context) const {
		InternalInstance my(this,e,o,instance,context);
		string stringToTrim = my.parm(1);
		Support::trim(stringToTrim);
		my.set(stringToTrim);
	}

}
