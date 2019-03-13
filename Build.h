//
// Created by Ben Griffin on 2019-03-06.
//

#ifndef MACROTEXT_BUILD_H
#define MACROTEXT_BUILD_H

#include <string>
#include <deque>
#include <set>
#include <unordered_map>
#include <map>
#include "support/Env.h"
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
};

struct BuildUser {
	size_t		id;					//eg. 2
	std::string username;   		//eg. bgriffin
	std::string name;				//eg. Ben Griffin
	bool 		found;				//exists in database.
	UserMay		may;
	std::unordered_map<size_t,UserMay> teams;
	BuildUser();
	void load(Support::Messages&,Support::Db::Connection&);
	bool checkTeam(size_t,Support::buildType);
	bool mayTeamEdit(size_t);
};

class Build {

private:
	size_t currentLangID;
	size_t currentTechID;
	bool allTechs;
	bool allLangs;
	size_t currentPage;
	std::string currentSuffix;
	std::deque< std::pair<size_t,std::string> > languages;
	std::deque< std::pair<size_t,std::string> > technologies;
	bool mayBuild;

	Build();
	void loadLanguages(Support::Messages&,Support::Db::Connection&);
	void loadTechs(Support::Messages&,Support::Db::Connection&);

public:
	BuildUser	user;
	static Build& b();
	static Build& b(Support::Messages&,Support::Db::Connection*,long double);

	Support::buildArea current() const { return _current; }
	size_t page() const { return currentPage; }
	std::string suffix() const { return currentSuffix; }
	size_t tech() const { return technologies.empty() ? 0 : technologies.front().first; }
	std::string techName() const { return technologies.empty() ? "None" : technologies.front().second; }
	size_t lang() const { return languages.empty() ? 0 : languages.front().first; }
	std::string langName() const { return languages.empty() ? "None" : languages.front().second; }
	void setSuffix(std::string suffix) { currentSuffix = suffix; }
	void setPage(size_t page) { currentPage = page; }
	void setCurrent(Support::buildArea current) { _current = current; }
	void setNodes(Support::buildType,std::deque<size_t>&);

	void run();

	bool mayDefer();

	bool nextTech() {
		if(!technologies.empty()) {
			technologies.pop_front();
		}
		return ! technologies.empty();
	}

	bool nextLang() {
		if(!languages.empty()) {
			languages.pop_front();
		}
		return ! languages.empty();
	}

	void list(); //for checking stuff only.
	void close(Support::Messages&);

private:
	Support::buildArea _current;	//one of Final/Draft/?Test/
	std::set<Support::buildType> builds;
	std::map<Support::buildType,std::deque<size_t>> requestedNodes;
};

#endif //MACROTEXT_BUILD_H
