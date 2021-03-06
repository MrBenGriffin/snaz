//
// Created by Ben on 2019-01-29.
//

#ifndef MACROTEXT_CURRENT_H
#define MACROTEXT_CURRENT_H

#include "mt.h"
#include "node/Node.h"
#include "node/Metrics.h"

namespace mt {
	class Internal;
	using namespace Support;
	// Used by Internals to hold current evaluation,
	// So using a base class isn't so good.
	// and also manages common utility functions.
	class InternalInstance {
	private:
	public:
		const Internal* owner;
		const node::Metrics* metrics;// = context.back().second.metrics;
		size_t          min{0}, max{0}, count {0};
		MacroText*      output {nullptr};
		Instance*  		instancePtr {nullptr};
		const plist*    parms {nullptr};
		mstack*         context {nullptr};
		Messages*       errs;
		InternalInstance(const Internal*,Support::Messages&,MacroText&,Instance&,mstack&);
		~InternalInstance();

		void 			nParms(plist&,const nlist&);
		void 			generate(plist&,const MacroText*,const string,const string="");
		void 			generate(nlist&,const MacroText*,const string,const string="");

		bool			boolParm(size_t,bool=false); //a boolean value (with a default).
		bool			reverse(size_t);
		std::string     parm(size_t);
		const MacroText*    praw(size_t);
		void            expand(size_t);
		void            set(std::string);
		void            logic(bool,size_t);
		void            logic(size_t,size_t);
		void            logic(const std::string&,size_t);
		void            logic(long double,std::string,size_t);
		plist 			toNodeParms(const Internal*,deque<string>&,string,size_t = string::npos);
		const node::Node*					node(size_t, node::flavour = node::content);
		const node::Node* 					node(const string&, node::flavour = node::content);
		pair<const node::Node*,size_t>		nodePage(size_t);

	};

}

#endif //MACROTEXT_CURRENT_H

