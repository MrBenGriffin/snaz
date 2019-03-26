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

#include "BuildUser.h"

namespace Support {
	namespace Db {
		class Connection;
	}
}

//* +----+---------+----+-----------+----------+
//* | id | name    | ln | territory | encoding |
//* +----+---------+----+-----------+----------+
//* |  1 | English | en | GB        | UTF-8    |
//* +----+---------+----+-----------+----------+

struct Language {
	std::string name;
	std::string ln;
	std::string territory;
	std::string encoding;
};

class Build {

public:
	static constexpr long double version = 2019.031301;
private:
	bool allTechs;
	bool allLangs;
	bool full;
	bool lock;
	size_t currentPage;
	std::string currentSuffix;
	Support::Db::Connection* sql;
	std::deque< std::pair<size_t,Language> > languages;
	std::deque< std::pair<size_t,std::string> > technologies;

	Build();
	void loadLanguages(Support::Messages&,Support::Db::Connection&);
	void loadTechs(Support::Messages&,Support::Db::Connection&);
	bool setLock(Support::Messages&,Support::Db::Connection&);
	void doParse(Support::Messages&,Support::Db::Connection&);

	void build(Support::Messages&,Support::Db::Connection&);
	void global(Support::Messages&,Support::Db::Connection&);
	void langs(Support::Messages&,Support::Db::Connection&);
	void techs(Support::Messages&,Support::Db::Connection&);
	void files(Support::Messages&,Support::Db::Connection&);


	static void releaseLock(int);

public:
	BuildUser	user;
	static Build& b();
	static Build& b(Support::Messages&);
	void run(Support::Messages&,Support::Db::Connection*);

	Support::buildKind current() const { return _current; }
	size_t page() const { return currentPage; }
	std::string suffix() const { return currentSuffix; }
	size_t tech() const { return technologies.empty() ? 0 : technologies.front().first; }
	std::string techName() const { return technologies.empty() ? "None" : technologies.front().second; }
	size_t lang() const { return languages.empty() ? 0 : languages.front().first; }
	std::string langName() const { return languages.empty() ? "None" : languages.front().second.name; }
	void setSuffix(std::string suffix) { currentSuffix = suffix; }
	void setPage(size_t page) { currentPage = page; }
	void setCurrent(Support::buildKind current) { _current = current; }
	void setNodes(Support::buildType,std::deque<size_t>&);

	void breakLock() { lock = false; }

	bool mayDefer();
	void list(); //for checking stuff only.
	void close(Support::Messages&);


private:
	Support::buildKind _current;	//one of Final/Draft/Test/Parse
	std::set<Support::buildType> builds;
	std::map<Support::buildType,std::deque<size_t>> requestedNodes;
};

#endif //MACROTEXT_BUILD_H
