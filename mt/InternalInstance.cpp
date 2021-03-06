//
// Created by Ben on 2019-01-29.
//

#include <stdexcept>
#include <support/Convert.h>
#include "InternalInstance.h"
#include "node/Node.h"
#include "node/Metrics.h"
#include "mt/Definition.h"

namespace mt {

	InternalInstance::~InternalInstance() {
		context->pop_front(); //Construct the Carriage for this.
	}

//	c.emplace_front(make_pair(thing,&i)); //Construct the Carriage for this.

	InternalInstance::InternalInstance(const Internal *thing,Messages& e,MacroText &o, Instance& i, mstack &c) :
			owner(thing),output(&o), instancePtr(&i), context(&c), errs(&e) {
		parms = &(instancePtr->parms);
		context->emplace_front(make_pair(thing,&i)); //Construct the Carriage for this.

		count = parms->size() == 1 ? parms->front().empty() ? 0 : 1 : parms->size();
		min = thing->minParms;
		max = thing->maxParms;
		metrics = instancePtr->metrics; //context->back().second.metrics;
		if(metrics == nullptr) {
			e << Message(warn,"InternalInstance Constructor: instance had no metrics, which cannot be right?");
			throw runtime_error("No Metrics.");
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
			e.push(Message(range,err.str()));
			thing->doTrace(e,*context);
			e.pop();
			count = max;
//			throw runtime_error("No Metrics.");
		}
	}

	const node::Node* InternalInstance::node(size_t ref, enum node::flavour flavour) {
		return node(parm(ref), flavour);
	}

	const node::Node* InternalInstance::node(const string& ref, enum node::flavour flavour) {
		if(metrics) {
			switch(flavour) {
				case node::taxon:
					return metrics->taxon(*errs,ref,context);
					break;
				case node::content:
					return metrics->byPath(*errs,ref,context);
					break;
				case node::suffix:
					return nullptr;
					break;
			}
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

	plist InternalInstance::toNodeParms(const Internal* parent,deque<string>& list,string sort,size_t maxSize) {
		deque<const node::Node*> nodes;
		for(auto& i : list) {
			auto* node = metrics->byPath(*errs,i,context);
			if(node != nullptr) {
				nodes.push_back(node);
			}
		}
		parent->doSort(*errs,nodes,sort);
		//resize after sort!
		if(maxSize < nodes.size()) {
			nodes.resize(maxSize);
		}
		plist result;
		nParms(result,nodes);
		return result;
	}

	void InternalInstance::nParms(plist& result,const nlist& nodes) {
		for(auto& i : nodes) {
			MacroText parm;
			parm.add(i->ids());
			result.emplace_back(std::move(parm));
		}
	}

	void InternalInstance::generate(nlist& nodes,const MacroText* program,const string valSub,const string iteSub) {
		if(program != nullptr && !program->empty()) { // from an empty parm..
			Definition def(owner->name() + " expansion");
			program->doFor(*errs,def.expansion,{valSub,iteSub},*context);
			node::Metrics local(metrics);
			auto instance = Instance(&local, true);
			nParms(instance.parms,nodes);
			def.expand(*errs,*output,instance,*context);
		}
	}

	void InternalInstance::generate(plist& parameters,const MacroText* program,const string valSub,const string iteSub) {
		if(program != nullptr && !program->empty()) { // from an empty parm..
			Definition def(owner->name() + " expansion");
			program->doFor(*errs,def.expansion,{valSub,iteSub},*context);
			node::Metrics local(metrics);
			auto instance = Instance(&local, true);
			instance.copy(&parameters);
			def.expand(*errs,*output,instance,*context);
		}
	}

	bool InternalInstance::boolParm(size_t i,bool _default) {
		if(i > count || i > parms->size()) {
			return _default;
		} else {
			std::ostringstream result;
			((*parms)[i - 1]).expand(*errs,result,*context);
			std::string val = result.str();
			return val == "true" || val == "1";
		}
	}

	bool InternalInstance::reverse(size_t i) {
		if(i > count || i > parms->size()) {
			return false;
		} else {
			std::ostringstream result;
			((*parms)[i - 1]).expand(*errs,result, *context);
			std::string val = result.str();
			return !val.empty() && (val[0] == 'R' || val[0] == 'r');
		}
	}

	std::string InternalInstance::parm(size_t i) {
		std::ostringstream result;
		if(i > count || i > parms->size()) {
			return "";
		} else {
			((*parms)[i - 1]).expand(*errs,result, *context);
			return result.str();
		}
	}

	const MacroText* InternalInstance::praw(size_t i) {
        return (count >= i || parms->size() >= i ) ? &(*parms)[i -1] : nullptr;
	}

	void InternalInstance::expand(size_t i) {
		if(count >= i && parms->size() >= i ) {
			((*parms)[i - 1]).expand(*errs,*output, *context);
		}
	}

	void InternalInstance::set(std::string str) {
		output->add(str);
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
