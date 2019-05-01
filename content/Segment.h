//
// Created by Ben on 2019-03-27.
//

#ifndef MACROTEXT_SEGMENT_H
#define MACROTEXT_SEGMENT_H


#include "support/Definitions.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "mt/mt.h"
#include "node/Node.h"
#include "node/Suffix.h"
#include "content/Layout.h"

using namespace Support;
using namespace Db;

namespace content {

	class Segment {

		static unordered_map<string,Segment*>  	refs;		//pointer to layout via reference. Loaded per tech.
		static unordered_map<size_t,Segment>   	segments;	//Where segment data is stored. Loaded once per build.
	public:
		static const Segment* get(Messages &,size_t);
		static const Segment* get(Messages &,const Layout*, string);
		static void load(Messages&,Connection&,buildKind);

		//only publicly available via const..
		size_t id;
		size_t type;	//this is the numeric version of the sig.
//		string ref;
		string name;
		string kind;
		string  sig;	//eg SEGTM (shoud work this out as an enum...)
		mt::mtext nl; 	//Parsed newline.

	};
}

#endif //MACROTEXT_SEGMENT_H
