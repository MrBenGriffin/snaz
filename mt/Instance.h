//
// Created by Ben Griffin on 2019-02-07.
//

#ifndef MACROTEXT_INSTANCE_H
#define MACROTEXT_INSTANCE_H

#include <string>
#include <ostream>
#include <vector>
#include <memory>

#include "mt/using.h"
#include "mt/Injection.h"
#include "node/Node.h"
#include "node/Metrics.h"

namespace mt {

	class forStuff {
	private:
		forStuff() = default;
	public:
		string nodeSym;
		string iterSym;
		Injection nodeInj;
		Injection iterInj;
		forStuff(const std::string&,const std::string&);
		forStuff(const forStuff&); //copy constructor
	};

	class Instance {
	private:
	public:
		plist parms;			//std::deque<MacroText>*
		bool generated;     	//internal generation via e.g. iForX
		iteration it = {0, 0};
		std::unique_ptr<forStuff> myFor;
		node::Metrics* metrics;

		size_t size();
		Instance(const Instance &);
		~Instance();
		Instance(const plist*,node::Metrics*, bool= false);
		Instance(const plist*, std::unique_ptr<forStuff>&, node::Metrics*);
		explicit Instance(node::Metrics*);
		void copy(const plist *);
		void tidy();

	};
}


#endif //MACROTEXT_INSTANCE_H
