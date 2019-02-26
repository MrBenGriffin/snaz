//
// Created by Ben on 2019-01-29.
//

#include <support/Convert.h>
#include "InternalInstance.h"

namespace mt {

	InternalInstance::InternalInstance(const Internal *thing,Messages& e,mtext &o, Instance &i, mstack &c) :
		owner(thing),output(&o), instance(&i), context(&c), errs(&e) {
		parms = i.parms;
		count = parms->size();
		min = thing->minParms;
		max = thing->maxParms;
		if(!thing->inRange(count)) {
			ostringstream err;
			err << "Range Error in ‘" << thing->name() << "’; " << count << " parameters found. But this macro requires";
			if (max == INT_MAX) {
				err << " at least " << min << " parameters.";
			} else {
				err << " from " << min << " to " << max << " parameters.";
			}
			e << Message(range,err.str());
		}
	}

	void InternalInstance::generate(plist& list,const mtext* program,string value,string count) {
		if(program != nullptr) { // from an empty parm..
			Instance i(&list,{1,list.size()},true); //set as generated.
			i.iValue = value;
			i.iCount = count;
			Definition macro(*program);
			macro.expand(*errs,*output,i,*context);
		}
	}

	bool InternalInstance::boolParm(size_t i,bool _default) {
		if(i > parms->size()) {
			return _default;
		} else {
			std::ostringstream result;
			Driver::expand((*parms)[i - 1],*errs,result, *context);
			std::string val = result.str();
			return val == "true" || val == "1";
		}
	}


	std::string InternalInstance::parm(size_t i) {
		std::ostringstream result;
		if(i > parms->size()) {
			return "";
		} else {
			Driver::expand((*parms)[i - 1],*errs,result, *context);
			return result.str();
		}
	}

	const mtext* InternalInstance::praw(size_t i) {
		const mtext* m = nullptr;
		if(parms->size() >= i) {
			m = &((*parms)[i - 1]);
		}
		return m;
	}

	void InternalInstance::expand(size_t i) {
		Driver::expand((*parms)[i - 1],*errs, *output, *context);
	}

	void InternalInstance::set(std::string str) {
		output->emplace_back(Text(str));
	}

	//offset points at the position of the TRUE parm.
	void InternalInstance::logic(bool equals,size_t offset) {
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

	//offset is the 1-indexed parameter of the parameter to convert to a number.
	void InternalInstance::logic(size_t base,size_t offset) {
		if(count < offset) {
			set(Support::tostring(base));
		} else {
			size_t test = Support::natural(parm(offset));
			logic(base == test,offset+1);
		}
	}

	//this defaults to outputting basis, unless offset is smaller than count..
	//offset: 1-indexed parm (where basis was)
	//eg iLayout(I0,Foo,T,F). =>> logic(Foo,2)
	void InternalInstance::logic(std::string& left,size_t offset) {
		if(count < offset) {
			set(left);
		} else {
			std::string right = parm(offset);
			logic(left == right,offset+1);
		}
	}
}