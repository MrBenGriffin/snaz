//
// Created by Ben on 2019-03-14.
//


#include <sstream>
#include "Definitions.h"

namespace Support {
	using namespace std;

	buildKind::buildKind(buildKindValue v) : value{v} {}
	bool buildKind::operator==(buildKindValue v) { return value == v; }
	bool buildKind::operator!=(buildKindValue v) { return value != v; }
	buildKind::operator buildKindValue() const { return value; }
	buildKind::operator std::string() const {
		switch (value) {
			case parse: return "parse";
			case final: return "final";
			case draft: return "draft";
			case test:  return "test";
		}
	}

}
