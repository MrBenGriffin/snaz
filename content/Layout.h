//
// Created by Ben on 2019-03-27.
//

#ifndef MACROTEXT_LAYOUT_H
#define MACROTEXT_LAYOUT_H


#include "support/Definitions.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "node/Node.h"
#include "node/Suffix.h"
#include "mt/mt.h"

#include "content/Template.h"
#include "content/Segment.h"

using namespace Support;
using namespace Db;

namespace content {
	class Layout {
		string ref;
		bool buildPoint;
		deque<const Template*> templates;
		deque<const Segment*> segments;
		unordered_map<string,const Segment*> segRefs;  //This is the layout's specific name for a segment.

		static unordered_map<string,Layout*>  		refs;				//pointer to layout via reference. Loaded per tech.
	static unordered_map<size_t,Layout>   		layouts;			//Where the templates are stored. Loaded per tech.
	public:
	static void load(Messages&,Connection&,size_t,buildKind);		//Per Tech.
	static const Layout* get(Messages&,size_t);
	static const Layout* get(Messages&,string);
};
}


#endif //MACROTEXT_LAYOUT_H
