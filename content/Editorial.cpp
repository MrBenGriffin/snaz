//
// Created by Ben on 2019-03-28.
//

#include "Editorial.h"
#include "Layout.h"
#include "Template.h"
#include "Segment.h"

#include "support/Timing.h"
#include "support/Convert.h"

using namespace node;
using namespace Support;

namespace content {

	Editorial::Editorial() : query(nullptr),lang(0),kind(final) {
	}

	Editorial &Editorial::e() {
		static Editorial singleton;
		return singleton;
	}

	void Editorial::unload(Messages &errs, Connection &sql) {
		contentStore.clear();
		qIndexes.clear();
		sql.dispose(query);
	}

	void Editorial::storeBuilt(Messages& errs) {

	}

	void Editorial::set(Messages &errs, Connection &sql,size_t langID, buildKind _kind) {
		Timing &times = Timing::t();
		if (times.show()) { times.set("Editorial Index"); }
		if(lang != 0) {
			storeBuilt(errs);
			sql.dispose(query); //reset.
			contentStore.clear();
			qIndexes.clear();
		}
		lang = langID; kind = _kind;
		ostringstream str,content;
		string source = (kind == draft ? "v.content" : "c.pcontent");
		content << "case s.type when 2 then sv.content when 5 then from_unixtime(" << source << ",'%Y %m %d %H %i %S') else " << source << " end";
		string pubdate = (kind == draft ? "from_unixtime(c.moddate,'%Y %m %d %H %i %S')" : "from_unixtime(c.pubdate,'%Y %m %d %H %i %S')");
		string editor  = (kind == draft ? "v.editor" : "c.editor");
		str << 	"select c.node,s.id as segment," << content.str() << " as content," << editor << " as editor, " << pubdate <<" as pubdate from "
				"bldnode n,bldnodelang a,bldlayoutsegments l,bldsegment s,bldcontent c";

		if (kind == draft) {
			str << ",bldcontentv v left join bldsegvalue sv on sv.id=v.content where v.id=c.version and a.used != ''";
		} else {
			str << " left join bldsegvalue sv on sv.id=c.pcontent where a.used in ('on','lk')";
		}
		str << " and a.node=n.id and s.id=l.sid and n.id=c.node and s.active='on' and s.type not in(4,11,15) and n.layout=l.lid and c.segment=l.sid"
		" and c.language=" << lang;


		// All the above is a bit mangled.. but we have the fields, the content, and the version/content alternatives.
		// The above gives us node,segment,content,editor,pubdate for this language.
		if (sql.dbselected() ) {
			if(sql.query(errs,query,str.str()) && query->execute(errs)) {
				size_t index = 1;
				while(query->nextrow()) {
					pair<size_t, size_t> ident;
					query->readfield(errs, "node", ident.first);
					query->readfield(errs, "segment", ident.second);
					qIndexes.emplace(ident,index++);
				}
			}
		}
		if (times.show()) { times.use(errs,"Editorial Index"); }
	}

	void Editorial::get(Messages &errs, const node::Content* node, const Segment* segment) {

	}


}
