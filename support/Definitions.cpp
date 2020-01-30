//
// Created by Ben on 2019-03-14.
//


#include <sstream>
#include "Definitions.h"

namespace Support {
	using namespace std;

	//• buildKind
	buildKind::buildKind(buildKindValue v) : value{v} {}
	bool buildKind::operator==(buildKindValue v) { return value == v; }
	bool buildKind::operator!=(buildKindValue v) { return value != v; }
	buildKind::operator buildKindValue() const { return value; }
	buildKind::operator std::string() const {
		switch (value) {
			case parse: return "parse";
			case define: return "define";
			case final: return "final";
			case draft: return "draft";
			case test: return "test";
		}
		return "test";
	}

	/** This is used by storage (bldvar table enum). If bldvar is changed, this needs to be changed too **/
	const std::string buildKind::bldvar() const {
		switch (value) {
			case final: return "pub";
			case draft: return "dev";
//			case test: return "dev";
			default: break;
		}
		return "";
	}

	buildKind::operator buildArea() const {
		switch (value) {
			case final:
				return Final;
			case draft:
				return Draft;
			case parse:
			case define:
				return Parse;
			case test:
				return Testing;
		}
		return Testing;
	}

	//• buildType
	buildType::buildType(buildTypeValue v) : value{v} {}
	bool buildType::operator==(buildTypeValue v) { return value == v; }
	bool buildType::operator!=(buildTypeValue v) { return value != v; }
	buildType::operator buildTypeValue() const { return value; }
	buildType::operator std::string() const {
		switch (value) {
			case Full: return "Full";
			case Branch: return "Branch";
			case Descendants: return "Descendants";
			case Single: return "Single";
		}
		return "Full";
	}

}
