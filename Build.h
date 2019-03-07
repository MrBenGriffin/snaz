//
// Created by Ben Griffin on 2019-03-06.
//

#ifndef MACROTEXT_BUILD_H
#define MACROTEXT_BUILD_H

#include <string>
#include <deque>
#include <set>
#include "support/Env.h"

namespace Support {
	class Messages;
	namespace Db {
		class Connection;
	}
}

class Build {

private:
	Support::buildArea _current;	//one of Final/Draft/?Test/
	size_t currentLangID;
	size_t currentTechID;
	size_t currentPage;
	std::string currentSuffix;
	std::set<size_t> userTeams;
	std::deque<size_t> languages;
	std::deque<size_t> technologies;
	std::deque<size_t>::const_iterator langCursor;
	std::deque<size_t>::const_iterator techCursor;

	Build();

public:
	static Build& b();

	Support::buildArea current() const { return _current; }
	size_t page() const { return currentPage; }
	std::string suffix() const { return currentSuffix; }
	size_t currentTech() const { return *langCursor; }
	size_t currentLang() const { return *techCursor; }
	void setSuffix(std::string suffix) { currentSuffix = suffix; }
	void setPage(size_t page) { currentPage = page; }
	void setCurrent(Support::buildArea current) { _current = current; }
	void addTech(size_t id) { technologies.push_back(id); }
	void addLang(size_t id) { languages.push_back(id); }

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

	void load(Support::Messages&, Support::Db::Connection&);
};


#endif //MACROTEXT_BUILD_H
