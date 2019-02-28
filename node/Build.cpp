//
// Created by Ben on 2019-02-28.
//

#include "Build.h"
#include "support/Timing.h"
#include "support/Convert.h"
#include "support/Env.h"
#include "support/db/Query.h"

void Build::inittaxfields() {
	tax_fields.insert(refmaptype::value_type("id",0));
	tax_fields.insert(refmaptype::value_type("scope",1));
	tax_fields.insert(refmaptype::value_type("linkref",2));
	tax_fields.insert(refmaptype::value_type("tier",3));
	tax_fields.insert(refmaptype::value_type("team",4));
	tax_fields.insert(refmaptype::value_type("title",5));
	tax_fields.insert(refmaptype::value_type("shorttitle",6));
	tax_fields.insert(refmaptype::value_type("classcode",7));
	tax_fields.insert(refmaptype::value_type("synonyms",8));
	tax_fields.insert(refmaptype::value_type("article",9)); //descr
}

void Build::loadLayouts(Messages& errs) {
	Timing& timer = Timing::t();
	Env& env = Env::e();
	if (errs.verbosity() > 3) errs << Message(info,"Loading layouts and templates.");
	if (timer.showTiming) { timer.setTiming('c',"loadLayouts"); }
	if (mysql->dbselected() && mysql->table_exists(errs,"bldlayout") && mysql->table_exists(errs,"bldlayouttechs") && mysql->table_exists(errs,"bldtemplate")) {
		ostringstream qstr;
		mysql->lock(errs, "bldlayout l read,bldlayouttechs x read,bldtemplate t read" ); //everyone can read only..
		qstr << "select layout,templatelist,name from bldlayout l, bldlayouttechs x where x.layout=l.id and technology=" << env.technology().id;
		auto* q = mysql->query(errs,qstr.str());
		if ( q->execute(errs) ) {
			string f_layout_id,f_templatelist,f_name;
			layoutCount = 0;
			while(q->nextrow()) {
				q->readfield(errs,"layout",f_layout_id);			// bool.
				q->readfield(errs,"templatelist",f_templatelist);	// bool.
				q->readfield(errs,"name",f_name);					// bool.
				deque<size_t> templateList;
				size_t id = natural(f_layout_id);
				tolist( templateList,f_templatelist);
				layoutList.insert({id, templateList});
				layoutRefs.insert({f_name, id});
				layoutNames.insert({id,f_name});
				layoutCount++;
			}
			if (errs.verbosity() > 3) {
				ostringstream text; text << layoutCount << " layouts imported.";
				errs << Message(info,text.str());
			}
			qstr.str("");
			qstr << "select distinct(t.id) as id,t.templatemacro,t.suffix,t.break,t.comment from bldtemplate t,bldlayouttechs x where ";
			qstr <<  "find_in_set(t.id,x.templatelist) != 0 and x.technology=" << env.technology().id;
			q->setquery(qstr.str());
			size_t templateCount;
			if ( q->execute() ) {
				templateCount = 0;
				string f_id,f_comment;
				while(q->nextrow()) {
					FileTemplate n;
					q->readfield("id",f_id);				//discarding bool.
					q->readfield("templatemacro",n.str);	//discarding bool.
					q->readfield("suffix",n.suffix);		//discarding bool.
					q->readfield("break",n.br);				//discarding bool.
					q->readfield("comment",f_comment);		//discarding bool.
					size_t id = natural(f_id);
					templateList.insert({id, pair<string, FileTemplate >(f_comment,n)});
					templateCount++;
				}
				bld->Template = templateCount;
				if (errs.verbosity() > 3)
					*Logger::log << Log::info << templateCount << " templates imported. " << Log::end;
			} else {
				*Logger::log << Log::error << "Build::loadLayouts: DB Error while loading templates." << Log::end;
			}
		} else {
			*Logger::log << Log::error << "Build::loadLayouts: DB Error while loading layouts." << Log::end;
		}
		bld->unlock();
	}
	if (showTiming) { *Logger::log << Log::timing << Log::Ctime << "loadLayouts" << "loadLayouts()" << Log::end; }
	if (errs.verbosity() > 3) {
		*Logger::log << O << Log::info << "Layouts and templates loaded." << Log::end;
	}
}

void Build::init(Messages& errs,Connection& dbc) {
	mysql = &dbc;
	loadLayouts(errs);
}

Build::Build() {
	layoutCount = 0;
	roott = nullptr;
	rootc = nullptr;
	roots = nullptr;
}

Build& Build::b() {
	static Build singleton;
	return singleton;
}


Node* Build::current(Node::kind tree) {
	if (tree == Node::page && !node_stack.empty()) {
		return node_stack.back();
	} else {
		switch(tree) {
			case Node::page:	return rootc;
			case Node::tax: 	return roott;
			case Node::file:	return roots;
		}
	}

}

Node* Build::node(size_t id,Node::kind tree) {
	return current(tree)->nodebyid(id);
}
