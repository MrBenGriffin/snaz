//
// Created by Ben on 2019-03-26.
//

#ifndef MACROTEXT_USER_H
#define MACROTEXT_USER_H

#include <string>
#include <deque>
#include <set>
#include <unordered_map>
#include <map>

#include "support/Definitions.h"
#include "support/Message.h"

namespace Support {
	namespace Db {
		class Connection;
	}
}

struct UserMay {
	bool edit;
	bool draft;
	bool final;
	bool draftDown;
	bool finalDown;
	UserMay();
};

class BuildUser {
	size_t		id;					//eg. 2
	std::string username;   		//eg. bgriffin
	std::string name;				//eg. Ben Griffin
	bool 		found;				//exists in database.
	std::unordered_map<size_t,UserMay> teams;
public:
	UserMay		may;
	BuildUser();
	size_t iD() { return id; ;}
	std::string givenName() { return name ;}
	std::string userName() { return username ;}
	void load(Support::Messages&,Support::Db::Connection&);
	bool check(size_t,Support::buildType) const;
	bool check(Support::buildType) const;
	bool mayTeamEdit(size_t);
};



#endif //MACROTEXT_USER_H
