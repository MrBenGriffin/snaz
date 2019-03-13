//
// Created by Ben Griffin on 2019-03-06.
//
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <map>

#include <sys/types.h>
#include <sys/stat.h>

#include "support/Env.h"
#include "support/Date.h"
#include "support/Message.h"
#include "support/Timing.h"
#include "support/db/Connection.h"
#include "support/db/Query.h"
#include "support/Infix.h"

#include "node/Node.h"

#include "Build.h"
#include "test.h"

using namespace std;
using namespace Support;
using namespace Support::Db;


Build& Build::b() {
	static Build singleton;
	return singleton;
}

Build::Build() : _current(final),lock(true),full(false),allTechs(false),allLangs(false),sql(nullptr),user() {
}

Build& Build::b(Messages &errs) {
	Build& bld = b();
	Timing& timer = Timing::t(); //initialise.
	return bld;
}

void Build::run(Messages &errs,Connection* _sql) {
	if (_sql == nullptr) {
		errs << Message(fatal,"Build needs an SQL connection.");
		return;
	} else {
		sql = _sql;
	}
	Env& env = Env::e();
	Date date;
	std::ostringstream str;
	str << "Builder v" << std::setprecision(16) << version << " - " << env.get("RS_SITENAME") << " - ";
	switch(_current) {
		case test: {
			str << "Test System; started on " << date.str() << " (UTC)";
			errs << Message(info,str.str());
			testing::group tests("tests/");        	// Set the working directory from the Run|Edit Configurations... menu.
			tests.title(std::cout, "Main");
			tests.load(std::cout,  "main", false);   // Boolean turns on/off success reports.
		} break;
		case draft: {
			str << "Draft Build; started on " << date.str() << " (UTC)";
			errs << Message(info,str.str());
			build(errs,*sql);
		} break;
		case final: {
			str << "Final Build; started on " << date.str() << " (UTC)";
			errs << Message(info,str.str());
			build(errs,*sql);
		} break;
		case parse: {
			doParse(errs,*sql);
		} break;
	}
}

void Build::build(Messages &errs,Connection& sql) {
	errs << Message(info,"Loading Builder Configuration");
	user.load(errs,sql);
	loadLanguages(errs,sql);
	loadTechs(errs,sql);
	full = allLangs && allTechs && requestedNodes.empty();
	if( (_current == final && ((full && user.may.final) || user.may.finalDown)) ||
		(_current == draft && ((full && user.may.draft) || user.may.draftDown)) ) {
		if(setLock(errs,sql)) {
			if(full) {
				errs << Message(info,"This is a full build.");
			} else {
				ostringstream log;
				log << "This is a partial build";
				if(allLangs) { log << ", using all languages"; }
				if(allTechs) { log << ", with all technologies"; }
				log << ".";
				errs << Message(info,log.str());
			}
			global(errs,sql);
			releaseLock(0);
		} else {
			ostringstream err;
			err << "A build is currently being run the user " << user.name;
			errs << Message(fatal,err.str());
		}
	} else {
		ostringstream err;
		err << "This build is not allowed by the user " << user.name;
		errs << Message(fatal,err.str());
	}
}

void Build::global(Messages& errs,Connection& sql) {
	Node::loadLayouts(errs,sql);
//	bld->createDirectory(bld->scrDir);  //Generate the scripts directory if it doesn't exist.
//	macro::load(sql,local);		//load
//	SuffixVal::load();

/*****/
//	BuildProtocol(Build::Open,finalscript);
//	bld->loadPersistance();
//	RunScript("PRE_PROCESSING_SCRIPT", "Pre Processor", errs);
//
//	bld->loadSegmentIDs();
//	bld->calcBuildScope();
//	bld->initRandom();
	langs(errs,sql);
//	BuildLanguages();						//Main process.
//	iMedia::move(errs);
//
//	storageResult script = bld->varStorage.find("FINAL_PROCESSING_SCRIPT");
//	if (script.found) {
//		finalscript = script.result;
//	}
//	bld->savePersistance();
//	bld->prunePersistance();
//	RunScript("POST_PROCESSING_SCRIPT", "Post Processor", errs);
//	RunScript("~POST_PROCESSING_SCRIPT", "Post Processor", errs);
//	errs.str(Logger::log);
//	bld->clearPersistance();
//	iMedia::close(); (not sure why this is here).
/******/

//SuffixVal::unload();
//  macro::terminate();			//unload
//do FINAL_PROCESSING_SCRIPT stuff here if it's a full, final build..

}

void Build::langs(Messages& errs,Connection& sql) {
	//for each language..
	techs(errs,sql);
	// .. and tidy
}

void Build::techs(Messages& errs,Connection& sql) {
	//for each tech..
	files(errs,sql);
	// .. and tidy
}

void  Build::files(Messages& errs,Connection& sql) {
	//for each requestedNodes (or everything if it's a full build).
}

void Build::doParse(Messages &errs, Connection&) {
	errs << Message(fatal,"Parse is not yet implemented.");
}

void Build::loadLanguages(Messages &errs, Connection& dbc) {
	map< size_t,string > qLangs;
	if (dbc.dbselected() && dbc.table_exists(errs,"bldlanguage")) {
		errs << Message(info,"Loading Languages");
		Query* query;
		size_t mode = _current == final ? 2 : 1;
		ostringstream str;
		str << "select id,name from bldlanguage where active='on' and bldmode >= " << mode << " order by id";
		if(dbc.query(errs,query,str.str()) && query->execute(errs)) {
			while(query->nextrow()) {
				size_t id; string name;
				query->readfield(errs,"id",id);
				query->readfield(errs,"name",name);
				qLangs.insert({id,name});
			}
			delete query; query= nullptr;
		}
		auto& langs = Env::e().langs();
		if(!langs.empty()) {
			for(size_t i : langs) {
				auto lng = qLangs.find(i);
				if(lng != qLangs.end()) {
					languages.push_back({i,lng->second});
				}
			}
			allLangs = languages.size() == qLangs.size();
		} else {
			allLangs = true;
			for(auto& i : qLangs) {
				languages.push_back(i);
			}
		}
		ostringstream msg;
		msg  << languages.size() << " Languag" << (languages.size()==1? "e":"es") << " Loaded";
		errs << Message(info,msg.str());
	}
}

void Build::loadTechs(Messages &errs, Connection& dbc) {
	map<size_t,string> qTechs;
	if (dbc.dbselected() && dbc.table_exists(errs,"bldtechnology")) {
		Query* query;
		if(dbc.query(errs,query,"select id,name from bldtechnology where used='on' order by id") && query->execute(errs)) {
			while(query->nextrow()) {
				size_t id; string name;
				query->readfield(errs,"id",id);
				query->readfield(errs,"name",name);
				qTechs.insert({id,name});
			}
			delete query; query= nullptr;
		}
	}
	auto& techs = Env::e().techs();
	if(!techs.empty()) {
		for(size_t i : techs) {
			auto lng = qTechs.find(i);
			if(lng != qTechs.end()) {
				technologies.push_back({i,lng->second});
			}
		}
		allTechs = technologies.size() == qTechs.size();
	} else {
		allTechs = true;
		for(auto& i : qTechs) {
			technologies.push_back(i);
		}
	}
	ostringstream msg;
	msg  << technologies.size() << " Technolog" << (technologies.size()==1? "y":"ies") << " Loaded";
	errs << Message(info,msg.str());
}

void Build::setNodes(Support::buildType style,std::deque<size_t>& list) {
	const auto& p = requestedNodes.find(style);
	if(p != requestedNodes.end()) {
		auto& current = (*p).second;
		for(auto& i : list) {
			current.push_back(i);
		}
	} else {
		requestedNodes.emplace(style,std::move(list));
		builds.insert(style);
	}
}

bool Build::mayDefer() {
	return requestedNodes.size() == 1 && requestedNodes.count(Singles) == 1;
}

void Build::list() {
	for(auto& i : requestedNodes) {
		switch (i.first) {
			case Branch: std::cout << "Branch Nodes"; break;
			case Descendants: std::cout << "Descendant Nodes:"; break;
			case Singles: std::cout << "Single Nodes:"; break;
		}
		for(auto& j : i.second) {
			std::cout << j << ",";
		}
		std::cout << std::endl;
	}
}

void Build::close(Support::Messages &errs) {
	Infix::Evaluate::shutdown();
	Timing& timer = Timing::t();
	if (timer.show()) { timer.get(errs,'b'); }
	errs.str(std::cout, true); //output always.
}

Support::buildArea Build::area() const {
	switch (_current) {
		case final:
			return Final;
		case draft:
			return Draft;
		case test:
			return Testing;
		case parse:
			return Parse;
	}
}

bool Build::setLock(Support::Messages& errs,Connection& dbc) {
	bool response = false;
	ostringstream str;
	string otherUser = "Unknown";
	signal(SIGINT, releaseLock);        // On a Ctrl-C, we must delete our lock file.
	signal(SIGTERM, releaseLock);
	if (!lock) { releaseLock(0); }
	if (dbc.dbselected() && dbc.table_exists(errs, "blddefs")) {
		size_t count = 0;
		Query *query;
		str << "select count(type) as locked,value from blddefs where type='LOCK" << (_current == draft ? "D" : "F")
			<< "' group by type,value";
		if (dbc.query(errs, query, str.str()) && query->execute(errs)) {
			while (query->nextrow()) {
				query->readfield(errs, "locked", count);
				query->readfield(errs, "value", otherUser);
			}
			delete query;
			query = nullptr;
		}
		str.str("");
		if (count == 0 || !lock) {
			str << "replace into blddefs set type='LOCK" << (_current == draft ? "D" : "F") << "',value=concat('"
				<< user.username << ", at ',now())";
			if (dbc.query(errs, query, str.str()) && query->execute(errs)) {
				response = true;
				delete query;
				query = nullptr;
			}
		} else {
			Env env = Env::e();
			string this_script,these_parms;
			env.get("SCRIPT_URI",this_script);    //discard existence
			env.get("QUERY_STRING",these_parms);  //discard existence
			errs << Message(error,"Builder Busy!");
			str << "Another instance of builder initiated by " << otherUser << " is currently in action.";
			errs << Message(error,str.str()); str.str("");
			errs << Message(info, "Only click on OVERRIDE if you KNOW that this event is caused by a fatal error, otherwise refresh this page. ");
			errs << Message(info, "This event is caused when someone else is building the site, a build has caused a fatal error leaving the lock active, or a timed build is occuring. ");
			str <<  this_script << "?-X+" << these_parms;
			errs << Message(link,str.str());
		}
	}
	return response;
}

extern "C"
void Build::releaseLock(int) {
	Build& build = b();
	Messages log;
	if (build.sql != nullptr) {
		if (build.sql->isopen() && build.sql->dbselected() && build.sql->table_exists(log, "blddefs"))  {
			Query* query = nullptr;
			ostringstream str;
			str << "delete from blddefs where type='LOCK" << ( build._current == draft ? "D" : "F") << "'";
			if (build.sql->query(log,query,str.str())) {
				query->execute(log);
				delete query;
			}
		}
	}
	log.str(std::cerr);
}

BuildUser::BuildUser() : id(0),username(""),name("Unrecognised Person"),found(false) {
	may.edit = false;  //go to editor page (in general)
	may.draft = false; //draft build all site
	may.final = false; //final build all site.
}

bool BuildUser::mayTeamEdit(size_t team) {
	auto t = teams.find(team);
	if(t == teams.end()) {
		return false;
	} else {
		return t->second.edit;
	}
}

UserMay::UserMay() : edit(false),draft(false),final(false),draftDown(false),finalDown(false) {}

bool BuildUser::checkTeam(size_t team,buildType what) {
	auto t = teams.find(team);
	if(t == teams.end()) {
		return false;
	} else {
		auto area = Build::b().current();
		auto& item = t->second;
		switch (area) {
			case Final: {
				switch(what) {
					case Branch:
						return (item.final && item.finalDown);
					case Descendants:
						return (item.finalDown);
					case Singles:
						return (item.final);
				}
			} break;
			case Draft: {
				switch(what) {
					case Branch:
						return (item.draft && item.draftDown);
					case Descendants:
						return (item.draftDown);
					case Singles:
						return (item.draft);
				}
			} break;
			case Editorial: {
				return item.edit;
			} break;
			default: return false;
		}
	}
}

void BuildUser::load(Support::Messages& errs,Support::Db::Connection& dbc) {
	Env env = Env::e();
	if (env.get("REMOTE_USER",username)) {
		dbc.escape(username);
		if (dbc.dbselected() && dbc.table_exists(errs,"blduser")) {
			Query* query;
			if(dbc.query(errs,query,"select id,concat(firstname,' ',name) as name from blduser where username='" + username + "'") && query->execute(errs)) {
				while(query->nextrow()) {
					found = true;
					query->readfield(errs,"id",id);
					query->readfield(errs,"name",name);
				}
				delete query; query= nullptr;
			}
			if(found) {
				ostringstream str;
//		*** Team-independent activities.
//		Build Public       | PBLD | May build the public site from the admin page (allows full build)
//		Build Site Preview | DBLD | May build the preview site from the admin page (allows full build)
//		General Edit	   | EDIT | Has access to editorial.
				str << "select a.vcode as code from blduserteamroles utr,bldroleactivities ra,bldactivity a where "
					   "ra.setting=2 and utr.member=" << id << " and utr.role=ra.role and ra.activity=a.id "
															   "and a.vcode in ('EDIT','DBLD','PBLD') group by code";
				if(dbc.query(errs,query,str.str()) && query->execute(errs)) {
					string code;
					while(query->nextrow()) {
						query->readfield(errs,"code",code);
						if(code == "EDIT") may.edit = true;
						if(code == "DBLD") { may.draft = true; may.draftDown = true; }
						if(code == "PBLD") { may.final = true; may.finalDown = true; }
					}
					delete query; query= nullptr;
				}
				str.str("");

				//Team-based activities. 'ETPS','ETPD','ETBP','ETBD','EDVT'
//		*** Team-based activities.
//		Publish Now        | ETPS | May build the current page to public
//		Publish Down       | ETPD | May build the current branch to public
//		Build Preview      | ETBP | May build the current page to preview
//		Build Preview Down | ETBD | May build the current branch for preview
//		May Edit this Node | EDVT | Has access to editorial.
				str << "select utr.team,a.vcode as code from blduserteamroles utr,bldroleactivities ra,bldactivity a "
					   "where ra.setting=2 and utr.member=" << id << " and utr.role=ra.role and ra.activity=a.id "
																	 " and a.vcode in ('ETPS','ETPD','ETBP','ETBD','EDVT') group by team,code";
				if(dbc.query(errs,query,str.str()) && query->execute(errs)) {
					size_t team;
					std::string code;
					while(query->nextrow()) {
						query->readfield(errs,"team",team);
						query->readfield(errs,"code",code);
						auto pos = teams.find(team);
						if(pos == teams.end()) {
							UserMay tmay;
							//'ETPS','ETPD','ETBP','ETBD','EDVT'
							if(code == "ETPS") tmay.final = true;
							if(code == "ETPD") tmay.finalDown = true;
							if(code == "ETBP") tmay.draft = true;
							if(code == "ETBD") tmay.draftDown = true;
							if(code == "EDVT") tmay.edit = true;
							teams.emplace(team,tmay);
						} else {
							if(code == "ETPS") pos->second.final = true;
							if(code == "ETPD") pos->second.finalDown = true;
							if(code == "ETBP") pos->second.draft = true;
							if(code == "ETBD") pos->second.draftDown = true; //		Build Preview Down
							if(code == "EDVT") pos->second.edit = true;
						}
					}
					delete query; query= nullptr;
				}
			}
		}
	} else {
		if (env.area() == Console) { // set 'user' up for Console use.
			found = true;
			id  =0;
			name= "CLI";
			may.edit = false;
			may.draft = true; may.draftDown = true;
			may.final = true; may.finalDown = true;
		}
	}

};
