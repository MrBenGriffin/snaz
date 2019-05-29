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
	private:
		forStuff() = default;
	public:
		~forStuff();
		std::vector<std::pair<std::string,std::string>> stuff;
		forStuff(const std::string&,const std::string&,size_t,size_t);
		forStuff(const std::string&,const std::string&);
		forStuff(const std::vector<std::pair<std::string,std::string>>&);
		void set(const std::string&,size_t);
	};

	class Instance {
	private:
		void copy(const plist *);
	public:
		plist parms;			//std::vector<mtext>
		bool generated;     	//internal generation via e.g. iForX
		iteration it = {0, 0};
		std::unique_ptr<forStuff> myFor;
		node::Metrics* metrics;

		size_t size();
		Instance(const Instance &);
		~Instance();
		Instance(plist,node::Metrics*, bool= false);
//		Instance(plist, iteration, node::Metrics*, bool= false);
		Instance(plist, std::unique_ptr<forStuff>&, node::Metrics*);
		Instance(std::unique_ptr<forStuff>&);
		explicit Instance(node::Metrics*);
		void tidy();

	};
}


#endif //MACROTEXT_INSTANCE_H
