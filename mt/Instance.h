//
// Created by Ben Griffin on 2019-02-07.
//

#ifndef MACROTEXT_INSTANCE_H
#define MACROTEXT_INSTANCE_H

#include <string>
#include <ostream>
#include <vector>

#include "mt/using.h"
#include "node/Node.h"
#include "node/Metrics.h"

namespace mt {

	class forStuff {
	public:
		std::vector<std::pair<std::string,std::string>> stuff;
		forStuff(const std::string&,const std::string&,size_t,size_t);
		forStuff(const std::string&,const std::string&);
		void set(const std::string&,size_t);
	};

	class Instance {
	private:
		void copy(const plist *);
	public:
		plist parms;			//std::vector<mtext>
		forStuff* myFor;
		bool generated;     	//internal generation via e.g. iForX
		iteration it = {0, 0};
		node::Metrics* metrics;

		size_t size();
		Instance(const Instance &) = default;
		Instance(plist, iteration, node::Metrics*, bool= false);
		Instance(plist, forStuff&, node::Metrics*);
		explicit Instance(node::Metrics*);
	};
}


#endif //MACROTEXT_INSTANCE_H
