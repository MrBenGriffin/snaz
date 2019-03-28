//
// Created by Ben on 2019-03-27.
//

#include "Segment.h"
#include "support/Timing.h"

using namespace node;
using namespace Support;

namespace content {

	unordered_map<string,Segment*>  		Segment::refs;				//pointer to layout via reference. Loaded per tech.
	unordered_map<size_t,Segment>   		Segment::segments;			//Where templates are stored. Loaded once per build.

	const Segment* Segment::get(Messages &errs,size_t id) {
		const Segment* result =  nullptr;
		auto found = segments.find(id);
		if(found != segments.end()) {
			result = &(found->second);
		} else {
			ostringstream err;
			err << "There is no segment with id " << id;
			errs << Message(range,err.str());
		}
		return result;
	}

	const Segment* Segment::get(Messages &errs,string ref) {
		const Segment* result =  nullptr;
		auto found = refs.find(ref);
		if(found != refs.end()) {
			result = found->second;
		} else {
			ostringstream err;
			err << "There is no segment with reference " << ref;
			errs << Message(range,err.str());
		}
		return result;
	}


	void Segment::load(Messages &errs, Connection &sql,buildKind) {

		//Set Segment Maps.
		/*
		 * cf. new bldsegtype table proposal.
		 Segment types (as declared in blddefs) and what happens to them.
		 name	XML encoded, expanded
		 SEG**	√		√
		 SEGXM	X		√
		 SEGRE	√		X
		 SEGRW	X		X
		 */
		Timing &times = Timing::t();
		if (times.show()) { times.set("Load Segments"); }

//	TODO:: Sort out segment types re. content storage.
//	19 | SEGAD | Admin Link           | T20       | 4    |
//	34 | SEGDC | Prompt - Segment     | W20       | 15   |
//	27 | SEGDC | Prompt - Layout      | W30       | 11   |

		if (sql.dbselected() && sql.table_exists(errs,"bldsegment") && sql.table_exists(errs,"blddefs") ) {
			string select = "select s.id,s.name,s.br,s.type,d.type as sign,d.value as kind from bldsegment s,blddefs d "
			" where left(d.type,3)='SEG' and d.code=s.type and d.type not in('SEGDC','SEGAD') and s.active='on' order by s.id";
			Query* q = nullptr;
			if (sql.query(errs,q,select) && q->execute(errs)) {
				while(q->nextrow()) {
					Segment basis;
					size_t id;
					string newline;
					q->readfield(errs,"id",id);
					q->readfield(errs,"name",basis.name);
					q->readfield(errs,"type",basis.type);
					q->readfield(errs,"kind",basis.kind);
					q->readfield(errs,"sign",basis.sig);
					q->readfield(errs,"br",newline); //newline macrotext.
					std::istringstream newlineCode(newline);
					basis.nl = mt::Driver(errs,newlineCode,mt::Definition::test_adv(newline)).parse(errs,true);

					auto ins = segments.emplace(id,std::move(basis));
					if(ins.second) {
						Segment* seg = &(ins.first->second);
						refs.emplace(seg->name,seg);
					}
				}
			}
			sql.dispose(q);
		}
		if (times.show()) { times.use(errs,"Load Segments"); }

	}

}
