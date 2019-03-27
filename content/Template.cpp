//
// Created by Ben on 2019-03-27.
//

#include "Template.h"
#include "node/Suffix.h"
#include "support/Timing.h"

using namespace node;
using namespace Support;

namespace content {
	bool 									Template::_show = false;
	unordered_map<size_t,Template>   		Template::templates;		//Where templates are stored. Loaded once per build.

	const Template* Template::get(Messages &errs,size_t id) {
		const Template* result =  nullptr;
		auto found = templates.find(id);
		if(found != templates.end()) {
			result = &(found->second);
		} else {
			ostringstream err;
			err << "There is no template with id " << id;
			errs << Message(range,err.str());
		}
		return result;
	}

	//Must happen after suffixes are loaded.
	void Template::load(Messages &errs, Connection &sql, buildKind kind) {
		Timing &times = Timing::t();
		if (times.show()) { times.set("Load Templates"); }
		Query* q = nullptr;
		if (sql.dbselected() && sql.table_exists(errs, "bldtemplate")) {
			string str = "select id,comment as name,templatemacro as macro,suffix,break from bldtemplate";
			Query* q = nullptr;
			if (sql.query(errs,q,str) && q->execute(errs)) {
				while(q->nextrow()) {
					Template basis;
					size_t id;
					string _macro;
					string _break;
					string _suffix;
					q->readfield(errs, "id", id);
					q->readfield(errs, "id", id);
					q->readfield(errs, "name", basis.name);
					q->readfield(errs, "suffix", _suffix);
					q->readfield(errs, "break", _break);
					basis.suffix = Suffix::ref(errs, _suffix);
					std::istringstream bodyCode(_macro);
					basis.code = mt::Driver(errs,bodyCode,mt::Definition::test_adv(_macro)).parse(errs,true);
					std::istringstream newlineCode(_break);
					basis.nl = mt::Driver(errs,newlineCode,mt::Definition::test_adv(_break)).parse(errs,true);
					templates.emplace(id,std::move(basis));
				}
			}
			sql.dispose(q);
		}
		if (times.show()) { times.get(errs,"Load Templates"); }
	}
}

