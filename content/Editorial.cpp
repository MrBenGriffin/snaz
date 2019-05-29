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

	void Editorial::unload(Messages &, Connection &sql) {
		contentStore.clear();
		qIndexes.clear();
		sql.dispose(query);
	}

	bool Editorial::sanity(Messages &errs, key& id, const node::Content* node, const Segment* segment) const {
		bool value = false;
		if(node && segment) {
			id = { node->id(), segment->id };
			value = true;
		} else {
			ostringstream errstr;
			if (node) {
				errstr << "While looking for content with node " << node->ref() << ", a segment was empty";
			} else {
				if(segment) {
					errstr << "While looking for content with segment " << segment->name << " the node was empty";
				} else {
					errstr << "While looking for content with segment, both node and segment were empty";
				}
			}
			errs << Message(error,errstr.str());
		}
		return value;
	}

	void Editorial::storeBuilt(Messages& errs) {
		errs << Message(error,"Editorial::storeBuilt is not yet implemented.");
	}

	void Editorial::set(Messages &errs, Connection &sql,size_t langID, buildKind _kind) {
		Timing &times = Timing::t();
		if (times.show()) { times.set("Editorial Index"); }
		sql.dispose(query); //reset.
		if(lang != 0) {
			storeBuilt(errs);
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
		" and c.language=" << lang; //limit 24 this big is too much. limit 23 is ok

		// All the above is a bit mangled.. but we have the fields, the content, and the version/content alternatives.
		// The above gives us node,segment,content,editor,pubdate for this language.
		if (sql.dbselected() ) {
			if(sql.query(errs,query,str.str()) && query->execute(errs)) {
				size_t index = 1; //Although mysql is 0-indexed, we deal with that internally.
				while(query->nextrow()) {
					key ident;
					query->readfield(errs, "node", ident.first);
					query->readfield(errs, "segment", ident.second);
					qIndexes.emplace(ident,index++);
				}
				query->resetRows(errs);
			} else {
				sql.dispose(query);
			}
		}
		if (times.show()) { times.use(errs,"Editorial Index"); }
	}

	bool Editorial::has(Messages &errs, const node::Content* node, const Segment* segment) const {
		bool value = false;
		key id;
		if(sanity(errs,id,node,segment)) {
			auto idx = qIndexes.find(id);
			value = idx != qIndexes.end();
		}
		return value;
	}


	pair<bool,const mt::MacroText*> Editorial::get(Messages &errs, const node::Content* node, const Segment* segment) {
		static mt::MacroText empty;
		pair<bool,const mt::MacroText*> value = { true, &empty};
		key id;
		if(sanity(errs,id,node,segment)) {
			auto cIndex = contentStore.find(id); //unordered_map< key, MacroText, hash_pair> contentStore;
			if(cIndex != contentStore.end()) {
				value.first  = cIndex->second.first;
				value.second = &(cIndex->second.second);
			} else {
				auto index = qIndexes.find(id);
				if(index != qIndexes.end()) {
					query->setRow(errs,index->second);
					string content;
					query->readfield(errs, "content", content);
					pair<bool,mt::MacroText> toCache={ false, mt::Driver::parse(errs,content,false) };
					toCache.first=(toCache.second.simple());
					auto idx = contentStore.emplace(id,std::move(toCache));
					value.first  = idx.first->second.first;
					value.second = &idx.first->second.second;
				}
			}
		}
		return value;
	}

	std::string Editorial::getMeta(Messages &errs, const node::Content* node, const Segment* segment,const std::string& field) const {
		std::string value;
		key id;
		if(sanity(errs,id,node,segment)) {
			auto index = qIndexes.find(id);
			if(index != qIndexes.end()) {
				query->setRow(errs,index->second);
				query->readfield(errs, field, value);
			}
		}
		return value;
	}

}
