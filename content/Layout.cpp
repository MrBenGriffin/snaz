//
// Created by Ben on 2019-03-27.
//

#include <sstream>

#include "Layout.h"
#include "Template.h"
#include "Segment.h"

#include "support/Timing.h"
#include "support/Convert.h"

#include "node/Content.h"

using namespace node;
using namespace Support;

namespace content {
unordered_map<string,Layout*>  			Layout::refs;		//Pointer to layout via reference. Loaded per tech.
unordered_map<size_t,Layout>   			Layout::layouts;	//Where the templates are stored. Loaded per tech.

size_t Layout::pages() const {
	return templates.size();
}

void Layout::compose(Messages& errs,Content& content,buildKind kind,size_t langID,size_t techID) {
	std::ostringstream msg; msg << "Layout " << ref;
	errs << Message(info,msg.str());
	for (auto* t : templates) {
		Template::templates[t->id].compose(errs,*this,content,kind,langID,techID);
	}
}

const Layout* Layout::get(Messages &errs,string ref) {
	const Layout* result =  nullptr;
	auto found = refs.find(ref);
	if(found != refs.end()) {
		result = found->second;
	} else {
		ostringstream err;
		err << "There is no layout with reference " << ref;
		errs << Message(range,err.str());
	}
	return result;
}

const Layout* Layout::get(Messages &errs,size_t id) {
	const Layout* result =  nullptr;
	auto found = layouts.find(id);
	if(found != layouts.end()) {
		result = &(found->second);
	} else {
		ostringstream err;
		err << "There is no layout with id " << id;
		errs << Message(range,err.str());
	}
	return result;
}

const Segment* Layout::segment(Messages &errs,string ref) const {
	/*
	 * get segment by ref. from here so we can use the local name if needs be.
	 */
	const Segment* value =  nullptr;
	auto found = segRefs.find(ref);
	if(found != segRefs.end()) {
		value = found->second;
	} else {
		value = Segment::get(errs,this,ref);
	}
	return value;
}

void Layout::load(Messages &errs, Connection &sql,size_t techID, buildKind) {
	Timing &times = Timing::t();
	if (times.show()) { times.set("Load Layouts"); }
	if(!refs.empty()) {
		refs.clear();	//This is called once per tech.
		layouts.clear();
	}
	if (sql.dbselected() && sql.table_exists(errs, "bldlayout") && sql.table_exists(errs, "bldlayouttechs") ) {
		Query* q = nullptr;
		ostringstream str;
		str << "select l.id,x.templatelist,l.name,buildpoint='on' as build from "
			   "bldlayout l, bldlayouttechs x where x.layout=l.id and technology=" << techID;
		if (sql.query(errs,q,str.str()) && q->execute(errs)) {
			while(q->nextrow()) {
				string templateList;
				deque<size_t> templates;
				Layout basis;
				q->readfield(errs, "id", basis.id);
				q->readfield(errs, "name", basis.ref);
				q->readfield(errs, "build", basis.buildPoint);
				q->readfield(errs, "templatelist", templateList);
				tolist(templates,templateList); //Converted to a deque of size_t.
				for(auto t : templates) {
					basis.templates.push_back(Template::get(errs,t));
				}
				auto ins = layouts.emplace(basis.id,std::move(basis));
				if(ins.second) {
					Layout* layoutPtr = &(ins.first->second);
					refs.emplace(layoutPtr->ref,layoutPtr);
				}
			}
		}
		sql.dispose(q); str.str("");
//	TODO:: Sort out segment types re. content storage.
//	19 | SEGAD | Admin Link           | T20       | 4    |
//	34 | SEGDC | Prompt - Segment     | W20       | 15   |
//	27 | SEGDC | Prompt - Layout      | W30       | 11   |

//	TODO:: Talk with Param about migrating segments to a per layout-technology rather than per layout configuration.
//The below has nothing to do with technology or languages (currently).
//It sort of makes sense to limit segments to a given technology, but currently they are aligned to a layout
//Regardless of the technology. It's a mistake, as it's the templates which drive segment usage.
		str << "select x.lid,x.sid,x.comment from bldlayoutsegments x,bldsegment s,blddefs d "
			   "where s.id=x.sid and s.active='on' and d.code=s.type "
			   "and left(d.type,3)='SEG' and d.type not in('SEGDC','SEGAD')";

		if (sql.query(errs,q,str.str()) && q->execute(errs)) {
			while(q->nextrow()) {
				size_t id;
				size_t seg;
				string segRef;
				q->readfield(errs, "lid", id);
				q->readfield(errs, "sid", seg);
				q->readfield(errs, "comment", segRef);
				auto found = layouts.find(id);
				if(found != layouts.end()) {
					Layout& layout = found->second;
					const Segment* segment = Segment::get(errs,seg);
					if(segment != nullptr) {
						layout.segments.push_back(segment);
						if(!segRef.empty()) {
							layout.segRefs.emplace(segRef,segment);
						}
					}
				}
			}
		}
		sql.dispose(q); str.str("");
	}
	if (times.show()) { times.get(errs,"Load Layouts"); }

}

}
