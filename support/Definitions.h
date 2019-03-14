//
// Created by Ben on 2019-03-14.
//

#ifndef MACROTEXT_ENUMS_H
#define MACROTEXT_ENUMS_H

#include <string>

namespace Support {

	enum buildSpace {
		Built, Temporary, Scripts, Media, Tests
	};  // for directory / file management.

	enum buildArea {
		Editorial, Final, Draft, Console, Release, Staging, Testing, Parse
	}; // for url generation, etc.

	enum buildKindValue {
		parse, final, draft, test
	}; // kinds of build that there are.. (each instance of builder runs one of these).

	// kinds of build that there are.. (each instance of builder runs one of these).
	class buildKind {
	private:
		buildKindValue value;
		buildKind() {}
	public:
		buildKind(buildKindValue); // : value{v} {}
		bool operator==(buildKindValue);// { return value == v; }
		bool operator!=(buildKindValue); // { return value != v; }
		operator buildKindValue() const; // { return value; }
		operator std::string() const;
	};

	enum buildType {
		Branch, Descendants, Singles
	}; // each build (final/draft) is made up of one or more of these, (or does everything)
}


#endif //MACROTEXT_ENUMS_H
