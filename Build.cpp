//
// Created by Ben Griffin on 2019-03-06.
//
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <map>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mt/Internal.h"
#include "mt/Driver.h"
#include "mt/Instance.h"
#include "mt/Definition.h"

#include "support/Env.h"
#include "support/File.h"
#include "support/Date.h"
#include "support/Message.h"
#include "support/Timing.h"
#include "support/Encode.h"
#include "support/db/Connection.h"
#include "support/db/Query.h"
#include "support/Infix.h"
#include "support/Media.h"

#include "node/Node.h"
#include "node/Suffix.h"
#include "node/Content.h"
#include "node/Taxon.h"

#include "content/Template.h"
#include "content/Segment.h"
#include "content/Layout.h"
#include "content/Editorial.h"

#include "Build.h"
#include "test.h"

using namespace std;
using namespace Support;
using namespace Support::Db;

Build& Build::b() {
	static Build singleton;
	return singleton;
}

Build::Build() : _current(final),lock(true),full(false),allTechs(false),allLangs(false),sql(nullptr),_media(nullptr),user() {
	_media = new Media();
}

Build::~Build() {
	delete _media;
}

//Build& Build::b(Timing& timer) {
//	timer = Timing::t(); //initialise.
//	return b();
//}

void Build::run(Messages &log,Connection* _sql) {
	if (_sql == nullptr) {
		log << Message(fatal,"Build needs an SQL connection.");
		return;
	} else {
		sql = _sql;
	}
	Env& env = Env::e();
	mt::Definition::startup(log); //initialise internals
	Date date;
	std::ostringstream str;
	str << "Building Site " << env.get("RS_SITENAME") << " " << string(_current) << " on " << date.str() << " (UTC)";
	log.push(Message(info,str.str()));
	switch(_current) {
		case test: {
			tests(log);
		} break;
		case parse: {
			doParse(log,*sql);
		} break;
		case draft:
		case final: {
			build(log);
		} break;
	}
	log.pop();
}
void Build::tests(Messages &errs) {
	Timing::t().set("Tests");
	mt::Definition::load(errs,*sql,_current); // This is quite slow.
	user.load(errs,*sql);
	loadLanguages(errs,*sql);
	loadTechs(errs,*sql);
	auto language = lang();

	node::Suffix().loadTree(errs,*sql,0, _current);
	content::Template::load(errs,*sql);
	content::Segment::load(errs,*sql,_current);
	node::Taxon().loadTree(errs,*sql,language, _current); 		//This is slow.
	node::Content().loadTree(errs,*sql,language,_current);		//This is fast.
	content::Editorial::e().load(errs,*sql,language,_current);
	_media->load(errs,sql);

	content::Layout::load(errs,*sql,tech(),_current);
	node::Content().setLayouts(errs);
	_media->setFilenames(errs);

	testing::group tests(errs,"tests/");        	 // Set the working directory from the Run|Edit Configurations... menu.
	tests.title(std::cout, "Main");
	tests.load(std::cout,  "main", false);   // Boolean turns on/off success reports.
	_media->close();

	content::Editorial::e().reset(errs,*sql);
	mt::Definition::shutdown(errs,*sql,_current); //bld->savePersistance(); prunePersistance(); clearPersistance();
	Timing::t().use(errs,"Tests");
	errs.str(cout);
}
void Build::build(Messages &errs) {
	if(setLock(errs,*sql)) {
		errs.push(Message(info,"Loading Configuration"));
		user.load(errs,*sql);
		loadLanguages(errs,*sql); //This will set Languages
		loadTechs(errs,*sql); //This will set the number of Technologies.
		errs.pop();
		full = allLangs && allTechs && requestedNodes.empty();
		if( (_current == final && ((full && user.may.final) || user.may.finalDown)) ||
			(_current == draft && ((full && user.may.draft) || user.may.draftDown)) ) {
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
			global(errs);
		} else {
			ostringstream err;
			err << "This build is not allowed by " << user.givenName();
			errs << Message(fatal,err.str());
		}
		releaseLock(0);
	} else {
		ostringstream err;
		err << "A build is currently being run by " << user.givenName();
		errs << Message(fatal,err.str());
	}
}

void Build::global(Messages& errs) {
	Env& env = Env::e();
	errs.push(Message(info,"Loading Macros, Suffixes, Scripts, Templates, Segments, and Media"));
	mt::Definition::load(errs,*sql,_current); // Set the internals.
	env.dir(Scripts).makeDir(errs);
	env.setWorkingDir(errs,full);
	node::Suffix().loadTree(errs,*sql,0, _current);
	content::Template::load(errs,*sql);
	content::Segment::load(errs,*sql,_current);
	_media->load(errs,sql);
	errs.pop();

//TODO:	RunScript("PRE_PROCESSING_SCRIPT", "Pre Processor", errs);
	langs(errs);
//
//TODO:	storageResult script = bld->varStorage.find("FINAL_PROCESSING_SCRIPT");
//	if (script.found) {
//		finalscript = script.result;
//	}

//TODO:	RunScript("POST_PROCESSING_SCRIPT", "Post Processor", errs);
//	RunScript("~POST_PROCESSING_SCRIPT", "Post Processor", errs);
//	errs.str(Logger::log);
// This is a post-processing script, NOT a suffix mapping script.
// Therefore, there is only one parameter - the path to the final web root directory


	_media->save(errs,sql,allTechs && full); //Condition for full reset.
//TODO:	iMedia::move(errs);
	_media->close();
	content::Editorial::e().reset(errs,*sql);
	mt::Definition::shutdown(errs,*sql,_current); //bld->savePersistance(); prunePersistance(); clearPersistance();
//TODO: do FINAL_PROCESSING_SCRIPT stuff here if it's a full, final build..

}

void Build::langs(Messages& errs) {
	static bool calculatedProgress(false);
	Timing& times = Timing::t();
	while (!languages.empty()) {
		auto lang = languages.front();
		times.set("Language " + lang.second.name);
		node::Taxon().loadTree(errs,*sql,lang.first, _current);
		node::Content::updateBirthAndDeath(errs,*sql,lang.first,_current); //* Per Language.
		node::Content::updateContent(errs,*sql,lang.first,_current); //this moves the latest version into bldcontent.
		//TODO:: Content APPROVERS??.
		node::Content().loadTree(errs,*sql,lang.first,_current);
		content::Editorial::e().load(errs,*sql,lang.first,_current);
		if(!calculatedProgress) {
			calculateNodesToBuild(errs);  //If we use the tech1 layoutlist, we get
			errs.calculateProgressSize(); //We can now determine progress!
			calculatedProgress = true;
			errs << Message(channel::adhoc,"Finished Loading",1.0L);
		}
		techs(errs);
		content::Editorial::e().store(errs,*sql,lang.first,_current);
		content::Editorial::e().reset(errs,*sql);
		times.use(errs,"Language " + lang.second.name);
		languages.pop_front();
		errs << Message(channel::language,lang.second.name,1.0L);
	}
}

void Build::techs(Messages& errs) {
	Timing& times = Timing::t();
	while (!technologies.empty()) {
		auto techno = technologies.front();
		times.set("Tech " + techno.second.name);
		content::Layout::load(errs,*sql,techno.first,_current);
		node::Content().setLayouts(errs);
		_media->setFilenames(errs);
		//.....
		try {
			files(errs);
			node::Suffix::processScss(errs);
			node::Suffix::processBatches(errs);
		} catch (...) {
			errs << Message(fatal,"exception thrown.");
		}
		//.....
		times.use(errs,"Tech " + techno.second.name);
		technologies.pop_front();
		errs << Message(channel::technology,techno.second.name,1.0L);
	}
}

void Build::files(Messages& errs) {
	if(requestedNodes.empty()) {
		node::Content::get(node::Content::root()->id()).generate(errs,Full);
	} else {
		for (auto t : requestedNodes) { //t.first is the buildType.
			for(auto n : t.second ) { // t.second is the deque of node IDs for this buildtype.
				const node::Content* node = node::Content::root()->content(errs,n);
				if(node != nullptr) {
					auto& base = node::Content::get(node->id());
					base.generate(errs,t.first);
				}
			}
		}
	}
}

void Build::calculateNodesToBuild(Messages& errs) const {
	std::set<size_t> counter;
	if(requestedNodes.empty()) {
		node::Content::get(node::Content::root()->id()).count(user,Full,counter);
	} else {
		for (auto t : requestedNodes) { //t.first is the buildType.
			for(auto n : t.second ) { // t.second is the deque of node IDs for this buildtype.
				const node::Content* node = node::Content::root()->content(errs,n);
				if(node != nullptr) {
					auto& base = node::Content::get(node->id());
					base.count(user,t.first,counter);
				}
			}
		}
	}
	errs.setProgressSize(Message::node,counter.size());
}

void Build::doParse(Messages &errs, Connection&) {
	errs << Message(fatal,"Parse is not yet implemented.");
}

void Build::loadLanguages(Messages &errs, Connection& dbc) {
	map< size_t,Language > qLangs;
	if (dbc.dbselected() && dbc.table_exists(errs,"bldlanguage")) {
		Query* query;
		size_t mode = _current == final ? 2 : 1;
		ostringstream str;
		str << "select id,name,ln,territory,encoding from bldlanguage where active='on' and bldmode >= " << mode << " order by id";
/**
	* +----+---------+----+-----------+----------+
	* | id | name    | ln | territory | encoding |
	* +----+---------+----+-----------+----------+
	* |  1 | English | en | GB        | UTF-8    |
	* +----+---------+----+-----------+----------+
 */
		if(dbc.query(errs,query,str.str()) && query->execute(errs)) {
			Language item;
			while(query->nextrow()) {
				string name;
				query->readfield(errs,"id",item.id);
				query->readfield(errs,"name",item.name);
				query->readfield(errs,"ln",item.ln);
				query->readfield(errs,"territory",item.territory);
				query->readfield(errs,"encoding",item.encoding);
				item.ref = item.ln;
				if(!item.territory.empty()) {
					item.ref.push_back('-');
					item.ref.append(item.territory);
					Support::tolower(item.ref);
				}
				qLangs.insert({item.id,item});
			}
//			delete query; query= nullptr;
		}
		dbc.dispose(query);
		auto& langs = Env::e().langs(); //As requested.
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
				languages.emplace_back(i);
			}
		}
		ostringstream msg;
		msg  << languages.size() << " Languag" << (languages.size()==1? "e":"es") << " Loaded";
		errs << Message(info,msg.str());
		errs.setProgressSize(Message::language,languages.size());
	}
}

void Build::loadTechs(Messages &errs, Connection& dbc) {
	map<size_t,Technology> qTechs;
	if (dbc.dbselected() && dbc.table_exists(errs,"bldtechnology")) {
		Query* query;
		if(dbc.query(errs,query,"select id,name from bldtechnology where used='on' order by id") && query->execute(errs)) {
			while(query->nextrow()) {
				Technology item;
				size_t id; string dir, name;
				query->readfield(errs,"id",item.id);
				query->readfield(errs,"id",item.dir);
				query->readfield(errs,"name",item.name);
				qTechs.insert({item.id,item});
			}
		}
		dbc.dispose(query);
	}
	auto& techs = Env::e().techs();
	if(!techs.empty()) {
		for(size_t i : techs) {
			auto tech = qTechs.find(i);
			if(tech != qTechs.end()) {
				technologies.push_back({i,tech->second});
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
	errs.setProgressSize(Message::technology,technologies.size());
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
	return requestedNodes.size() == 1 && requestedNodes.count(Single) == 1;
}

void Build::list() {
	for(auto& i : requestedNodes) {
		std::cout << std::string(i.first)<< " Nodes";
		for(auto& j : i.second) {
			std::cout << j << ",";
		}
		std::cout << std::endl;
	}
}

void Build::close(Support::Messages &log) {
	Infix::Evaluate::shutdown();
	Timing::t().get(log,'b');
	log << Message(channel::adhoc,"Build is finished",1.0L);
}

bool Build::setLock(Support::Messages& errs,Connection& dbc) {
	bool response = false;
	ostringstream str;
	string otherUser = "Unknown";
	signal(SIGINT, Build::releaseLock);        // On a Ctrl-C, we must delete our lock file.
	signal(SIGQUIT, Build::releaseLock);
	signal(SIGHUP, Build::releaseLock);
	signal(SIGTERM, Build::releaseLock);
	signal(SIGKILL, Build::releaseLock);        // Won't do much.
//	if (!lock) { releaseLock(0); }
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
		}
		dbc.dispose(query);

		str.str("");
		if (count == 0 || !lock) {
			str << "replace into blddefs set type='LOCK" << (_current == draft ? "D" : "F") << "',value=concat('"
				<< user.userName() << " at ',now())";
			if (dbc.query(errs, query, str.str()) && query->execute(errs)) {
				response = true;
				lock = true;
			}
			dbc.dispose(query); str.str("");
			str << "replace into blddefs set type='BUILD',value=" << errs.id();
			if (dbc.query(errs, query, str.str())) {
				query->execute(errs);
			}
			dbc.dispose(query);
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
void Build::releaseLock(int signal) {
	Build& build = b();
	Messages log(build.sql);
	if (signal != 0) {
		log << Message(fatal,"Build was terminated early - maybe a crash or override?!");
		log << Message(container,"Build is finished.");
	}
	if (build.sql != nullptr) {
		log.synchronise();
		if (build.sql->isopen() && build.sql->dbselected() && build.sql->table_exists(log, "blddefs"))  {
			Query* query = nullptr;
			ostringstream str;
			str << "delete from blddefs where type='BUILD' or type='LOCK" << ( build._current == draft ? "D" : "F") << "'";
			if (build.sql->query(log,query,str.str())) {
				query->execute(log);
			}
			build.sql->dispose(query);
		}
	} else {
		log.synchronise();
	}
}
