//
// Created by Ben on 2019-03-14.
//

#ifndef MACROTEXT_ENUMS_H
#define MACROTEXT_ENUMS_H

#include <string>

namespace Support {

	enum buildSub {
		Blobs, Content, Root
	};  // for directory / file management. Blobs are media directory.. ;-/

	enum buildSpace {
		Built, Temporary, Scripts, Tests
	};  // for directory / file management.

	enum buildArea {
		Editorial, Final, Draft, Console, Release, Staging, Testing, Parse
	}; // for url generation, etc.

	enum buildKindValue {
		parse, define, final, draft, test
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
		explicit operator std::string() const;
		explicit operator buildArea() const;
		const std::string bldvar()  const;
	};

	enum buildTypeValue {
		Full,Branch,Descendants,Single
	};

	// each build (final/draft) is made up of one or more of these, (or does everything).
	class buildType {
	private:
		buildTypeValue value;
		buildType() {}
	public:
		buildType(buildTypeValue); // : value{v} {}
		bool operator==(buildTypeValue);// { return value == v; }
		bool operator!=(buildTypeValue); // { return value != v; }
		operator buildTypeValue() const; // { return value; }
		explicit operator std::string() const;
	};

}


#endif //MACROTEXT_ENUMS_H
