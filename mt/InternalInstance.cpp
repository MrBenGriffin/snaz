//
// Created by Ben on 2019-01-29.
//

#include <support/Convert.h>
#include "InternalInstance.h"
#include "node/Node.h"
#include "node/Metrics.h"
//#include <cmath> //for nan test.

namespace mt {

	InternalInstance::InternalInstance(const Internal *thing,Messages& e,mtext &o, Instance &i, mstack &c) :
		owner(thing),output(&o), instance(&i), context(&c), errs(&e) {
		parms = &i.parms;
		count = parms->size() == 1 ? parms->front().empty() ? 0 : 1 : parms->size();
		min = thing->minParms;
		max = thing->maxParms;
		if(!context->empty()) {
			metrics = context->back().second.metrics;
			if(metrics == nullptr) {
				e << Message(warn,"InternalInstance Constructor: context.back had no metrics, which cannot be right?");
			}
		} else {
			e << Message(warn,"context was empty, which cannot be right?");
		}

		if(!thing->inRange(count)) {
			ostringstream err;
			err << "Range Error in ‘" << thing->name() << "’; " << count << " parameters found. But this macro requires";
			if (max == INT_MAX) {
				err << " at least " << min << " parameters.";
			} else {
				if(min == max) {
					err << " exactly " << min << " parameter" << ( min == 1 ? "." : "s.");
				} else {
					err << " from " << min << " to " << max << " parameters.";
				}
			}
			e << Message(range,err.str());
			count = max;
		}
	}

	const node::Node* InternalInstance::node(size_t ref) {
		return node(parm(ref));
	}

	const node::Node* InternalInstance::node(const string& ref) {
		if(metrics) {
			return metrics->byPath(*errs,ref);
		} else {
			return nullptr;
		}
	}

	pair<const node::Node*,size_t> InternalInstance::nodePage(size_t ref) {
		if(metrics) {
			return metrics->nodePage(*errs,parm(ref));
		} else {
			return {nullptr,0};
		}
	}

	plist InternalInstance::toNodeParms(const Internal* parent,vector<string>& list,string sort,size_t maxSize) {
		if(maxSize < list.size()) {
			list.resize(maxSize);
		}
		vector<const node::Node*> nodes;
		for(auto& i : list) {
			auto* node = metrics->byPath(*errs,i);
			if(node != nullptr) {
				nodes.push_back(node);
			}
		}
		parent->doSort(*errs,nodes,sort);
		plist result; //using plist=std::vector<mtext>;
		for(auto& i : nodes) {
			mtext parm;
			parm.emplace_back(new Text(i->ids()));
			result.emplace_back(std::move(parm));
		}
		return result;
	}

	void InternalInstance::generate(nlist& nodes,const mtext* program,const string value,const string count) {
		if(program != nullptr && !program->empty()) { // from an empty parm..
			Definition macro(*program,0,-1,true,false); //iterate,dont trim
			auto stuff = make_shared<forStuff>(value,count);
			plist parameters;
			for(auto& i: nodes) {
				mtext parm;
				parm.emplace_back(new Text(i->ids()));
				parameters.emplace_back(std::move(parm));
			}
			Instance i(parameters,stuff,nullptr); 	//set as generated.
			macro.expand(*errs,*output,i,*context);
		}
	}

	void InternalInstance::generate(plist& parameters,const mtext* program,const string value,const string count) {
		//parms,code,vToken,cToken
		if(program != nullptr && !program->empty()) { // from an empty parm..
			Definition macro(*program,0,-1,true,false); //iterate,dont trim
			auto stuff = make_shared<forStuff>(value,count);
			Instance i(parameters,stuff,nullptr); 	//set as generated.
			macro.expand(*errs,*output,i,*context);
		}
	}

	bool InternalInstance::boolParm(size_t i,bool _default) {
		if(i > count || i > parms->size()) {
			return _default;
		} else {
			std::ostringstream result;
			Driver::expand(*errs,(*parms)[i - 1],result, *context);
			std::string val = result.str();
			return val == "true" || val == "1";
		}
	}

	bool InternalInstance::reverse(size_t i) {
		if(i > count || i > parms->size()) {
			return false;
		} else {
			std::ostringstream result;
			Driver::expand(*errs,(*parms)[i - 1],result, *context);
			std::string val = result.str();
			return !val.empty() && (val[0] == 'R' || val[0] == 'r');
		}
	}

	std::string InternalInstance::parm(size_t i) {
		std::ostringstream result;
		if(i > count || i > parms->size()) {
			return "";
		} else {
			Driver::expand(*errs,(*parms)[i - 1],result, *context);
			return result.str();
		}
	}

	const mtext* InternalInstance::praw(size_t i) {
		const mtext* m = nullptr;
		if(count >= i || parms->size() >= i ) {
			m = &((*parms)[i - 1]);
		}
		return m;
	}

	void InternalInstance::expand(size_t i) {
		if(count >= i && parms->size() >= i ) {
			for(auto& token: (*parms)[i - 1]) {
				token->expand(*errs,*output, *context);
			}
//			Driver::expand(*errs, (*parms)[i - 1], *output, *context);
		}
	}

	void InternalInstance::set(std::string str) {
		output->emplace_back(new Text(str));
	}

	//offset points at the position of the TRUE parm.
	void InternalInstance::logic(bool equals,size_t offset)  {
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
	void InternalInstance::logic(size_t base,size_t offset)  {
		if(count < offset) {
			set(Support::tostring(base));
		} else {
			size_t test = Support::natural(parm(offset));
			logic(base == test,offset+1);
		}
	}

	void InternalInstance::logic(long double base,std::string format,size_t offset)  {
		if(count < offset) {
			set(Support::tostring(base,format));
		} else {
			auto l = Support::tostring(base,format);
			auto rs = parm(offset);      			//as a string..
			auto rd = Support::real(rs); 			//as a double..
			auto r = Support::tostring(rd,format);  //using the format..
			logic( l == r, offset + 1); //this means that we are looking for a precise -string- value in the test.
		}
	}

	//this defaults to outputting basis, unless offset is smaller than count..
	//offset: 1-indexed parm (where string to compare is)
	//eg iLayout(I0,Foo,T,F). =>> logic(Foo,2)
	void InternalInstance::logic(const std::string& left,size_t offset)  {
		if(count < offset) {
			set(left);
		} else {
			std::string right = parm(offset);
			logic(left == right,offset+1);
		}
	}
}