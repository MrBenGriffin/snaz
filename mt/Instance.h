//
// Created by Ben Griffin on 2019-02-07.
//

#ifndef MACROTEXT_INSTANCE_H
#define MACROTEXT_INSTANCE_H

#include <string>
#include <variant>
#include <ostream>
#include <vector>
#include <deque>

#include "mt/declarations.h"
#include "mt/Handler.h"
#include "node/Node.h"
#include "node/Metrics.h"


namespace mt {
	using mtext=std::deque<Token>;
	using parse_result=std::pair<bool,std::pair<mtext, bool>>;
	using iteration=std::pair<size_t,size_t>;
	using plist=std::vector<mtext>;
	using nlist=vector<const node::Node *>;
	using pos=std::pair<size_t,size_t>;

	class forStuff {
	public:
		std::vector<std::pair<std::string,std::string>> stuff;
		forStuff(const std::string&,const std::string&,size_t,size_t);
		forStuff(const std::string&,const std::string&);
		void set(const std::string&,size_t);
	};

	class Instance {
	private:
	public:
//
		const plist *parms = nullptr;
		forStuff* myFor;
		bool generated;     	//internal generation via e.g. iForX
		iteration it = {0, 0};
		node::Metrics* metrics;

		size_t size();
		Instance(const Instance &); //copy constructor.
		Instance(const plist *, iteration, node::Metrics*, bool= false);
		Instance(const plist *, forStuff&, node::Metrics*);
		explicit Instance(node::Metrics*);
	};

	//Handler is the variant of all token handlers..
	using Carriage= std::pair<Handler*, Instance>;
	using mstack=std::deque< Carriage >;

}


#endif //MACROTEXT_INSTANCE_H
