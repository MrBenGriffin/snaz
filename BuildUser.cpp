//
// Created by Ben on 2019-03-26.
//

#include "support/Env.h"
#include "support/Message.h"
#include "support/Timing.h"
#include "support/db/Connection.h"
#include "support/db/Query.h"
#include "support/Infix.h"

#include "Build.h"
#include "BuildUser.h"

using namespace std;
using namespace Support;
using namespace Support::Db;

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


bool BuildUser::check(buildType what) const {
	buildArea area = buildArea(Build::b().current());
	switch (area) {
		case Final: {
			switch (what) {
				case Branch:
					return (may.final && may.finalDown);
				case Descendants:
					return (may.finalDown);
				case Single:
					return (may.final);
				case Full:
					return (may.final);
			}
		}
			break;
		case Draft: {
			switch (what) {
				case Branch:
					return (may.draft && may.draftDown);
				case Descendants:
					return (may.draftDown);
				case Single:
					return (may.draft);
				case Full:
					return (may.draft);
			}
		}
			break;
		case Editorial: {
			return may.edit;
		}
			break;
		default:
			return false;
	}
}


bool BuildUser::check(size_t team,buildType what) const {
	auto t = teams.find(team);
	if(t == teams.end()) {
		return false;
	} else {
		buildArea area = buildArea(Build::b().current());
		auto& item = t->second;
		switch (area) {
			case Final: {
				switch(what) {
					case Branch:
						return (item.final && item.finalDown);
					case Descendants:
						return (item.finalDown);
					case Single:
						return (item.final);
					case Full:
						return (may.final);
				}
			} break;
			case Draft: {
				switch(what) {
					case Branch:
						return (item.draft && item.draftDown);
					case Descendants:
						return (item.draftDown);
					case Single:
						return (item.draft);
					case Full:
						return (may.draft);
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
			}
			dbc.dispose(query);
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
				}
				dbc.dispose(query);
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
				}
				dbc.dispose(query);

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
