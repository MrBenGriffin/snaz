//
// Created by Ben Griffin on 2019-03-06.
//
#include <iostream>
#include <sstream>

#include "support/Env.h"
#include "support/Message.h"
#include "support/db/Connection.h"
#include "support/db/Query.h"

#include "Build.h"

using namespace std;
using namespace Support;
using namespace Support::Db;

Build& Build::b() {
	static Build singleton;
	return singleton;
}

Build::Build() : _current(Final),mayBuild(false) {
}

Build& Build::b(Messages &errs, Connection* sql) {
	Build& bld = b();
	if(sql != nullptr) {
		bld.loadUserTeams(errs,*sql);
		bld.loadLanguages(errs,*sql);
		bld.loadTechs(errs,*sql);
	} else {
		errs << Message(fatal,"Build needs an SQL connection");
	}
	return bld;
}

void Build::loadUserTeams(Messages &errs, Connection& dbc) {
	Env env = Env::e();
	string user;
	if (env.get("REMOTE_USER",user)) {
		mayBuild = true;
		if (dbc.dbselected() && dbc.table_exists(errs,"blduser") && dbc.table_exists(errs,"blduserteamroles") && dbc.table_exists(errs,"bldteam")) {
			ostringstream querystr;
			querystr << "select ts.team as team from blduser tm,blduserteamroles ts,bldteam t where tm.username='" << user << "' and ts.team=t.id ";
			querystr << "and tm.id=ts.member and t.active='on' and tm.active='on' group by team";
			Query* q = dbc.query(errs,querystr.str());
			if ( q->execute(errs) ) {
				long f_team;
				while(q->nextrow()) {
					q->readfield(errs,"team",f_team);	//discarding bool.
					userTeams.insert((size_t)f_team);
				}
			}
			delete q;
		}
	} else {
		mayBuild = env.area() == Console;
	}
}

void Build::loadLanguages(Messages &errs, Connection& sql) {
//	std::deque<size_t> languages;
}

void Build::loadTechs(Messages &errs, Connection& sql) {
//	std::deque<size_t> technologies;
}

void Build::setNodes(buildStyle style,std::deque<size_t>& list) {
	const auto& p = nodes.find(style);
	if(p != nodes.end()) {
		auto& current = (*p).second;
		for(auto& i : list) {
			current.push_back(i);
		}
	} else {
		nodes.emplace(style,std::move(list));
		builds.insert(style);
	}
}

bool Build::mayDefer() {
	return nodes.size() == 1 && nodes.count(Singles) == 1;
}

void Build::list() {
	for(auto& i : nodes) {
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