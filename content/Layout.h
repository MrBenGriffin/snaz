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

#include "node/Content.h"

using namespace Support;
using namespace Db;

namespace content {
	class Template;
	class Segment;

	class Layout {
	public:
		static unordered_map<string,Layout*>  		refs;		//pointer to layout via reference. Loaded per tech.
		static unordered_map<size_t,Layout>   		layouts;	//Where the templates are stored. Loaded per tech.

		size_t id;
		string ref;
		bool buildPoint;
		vector<const Template*> templates;
		unordered_map<size_t,const Segment*> segments; //This is the layout's set of segments.
		unordered_map<string,const Segment*> segRefs;  //This is the layout's specific name for a segment.
		unordered_map<size_t,string> segIDRefs;        //This is the layout's specific name for a segment, via segment's id.

		static void load(Messages&,Connection&,size_t,buildKind);		//Per Tech.
		static const Layout* get(Messages&,size_t);
		static const Layout* get(Messages&,string);

		string segRef(Messages&,size_t) const;
		const Segment* segment(Messages&,string) const;
		pair<bool,const Segment*> segmentInScope(Messages&,string) const;
		size_t pages() const;

	};
}


#endif //MACROTEXT_LAYOUT_H
