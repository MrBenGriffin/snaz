//
// Created by Ben on 2019-03-27.
//

#include "Template.h"
//#include "content/Template.h"
#include "node/Suffix.h"
#include "support/Timing.h"
#include "mt/Driver.h"

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
	void Template::load(Messages &errs, Connection &sql) {
		Timing &times = Timing::t();
		if (times.show()) { times.set("Load Templates"); }
		if (sql.dbselected() && sql.table_exists(errs, "bldtemplate")) {
			string str = "select id,comment as name,templatemacro as macro,suffix,break from bldtemplate";
			Query* q = nullptr;
			if (sql.query(errs,q,str) && q->execute(errs)) {
				while(q->nextrow()) {
					Template basis;
					string _macro;
					string _break;
					string _suffix;
					q->readfield(errs, "id", basis.id);
					q->readfield(errs, "name", basis.name);
					q->readfield(errs, "suffix", _suffix);
					q->readfield(errs, "macro", _macro);
					q->readfield(errs, "break", _break);
					basis.suffix = Suffix::byRef(errs, _suffix);
					std::istringstream bodyCode(_macro);
					mt::Driver(errs,bodyCode,mt::Definition::test_adv(_macro)).parse(errs,basis.code,true);
					std::istringstream newlineCode(_break);
					mt::Driver(errs,newlineCode,mt::Definition::test_adv(_break)).parse(errs,basis.nl,true);
					templates.emplace(basis.id,std::move(basis));
				}
			}
			sql.dispose(q);
		}
		if (times.show()) { times.get(errs,"Load Templates"); }
	}
}

