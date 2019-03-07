//
// Created by Ben Griffin on 2019-03-06.
//

#ifndef MACROTEXT_BUILD_H
#define MACROTEXT_BUILD_H

#include <string>
#include <deque>
#include <set>
#include <map>
#include "support/Env.h"
#include "support/Message.h"

namespace Support {
//	class Messages;
	namespace Db {
		class Connection;
	}
}

class Build {

private:
	size_t currentLangID;
	size_t currentTechID;
	size_t currentPage;
	std::string currentSuffix;
	std::set<size_t> userTeams;
	std::deque<size_t> languages;
	std::deque<size_t> technologies;
	std::deque<size_t>::const_iterator langCursor;
	std::deque<size_t>::const_iterator techCursor;
	bool mayBuild;


	Build();
//	void load(Support::Messages&,Support::Db::Connection&);
	void loadUserTeams(Support::Messages&,Support::Db::Connection&);
	void loadLanguages(Support::Messages&,Support::Db::Connection&);
	void loadTechs(Support::Messages&,Support::Db::Connection&);


public:
	enum   buildStyle {Branch,Descendants,Singles};
	static Build& b();
	static Build& b(Support::Messages&,Support::Db::Connection*);

	Support::buildArea current() const { return _current; }
	size_t page() const { return currentPage; }
	std::string suffix() const { return currentSuffix; }
	size_t currentTech() const { return *langCursor; }
	size_t currentLang() const { return *techCursor; }
	void setSuffix(std::string suffix) { currentSuffix = suffix; }
	void setPage(size_t page) { currentPage = page; }
	void setCurrent(Support::buildArea current) { _current = current; }
	void setTechs(std::deque<size_t>& list) { technologies = std::move(list); }
	void setLangs(std::deque<size_t>& list) { languages = std::move(list); }
	void setNodes(buildStyle,std::deque<size_t>&);
	bool mayDefer();

	bool nextTech() {
		techCursor++;
		return techCursor == technologies.end();
	}

	bool nextLang() {
		langCursor++;
		return techCursor == technologies.end();
	}

	bool inUserTeams(size_t team) const {
		return userTeams.find(team) != userTeams.end();
	}

	void list(); //for checking stuff only.

private:
	Support::buildArea _current;	//one of Final/Draft/?Test/
	std::set<buildStyle> builds;
	std::map<buildStyle,std::deque<size_t>> nodes;
};


#endif //MACROTEXT_BUILD_H
