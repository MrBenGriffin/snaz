//
// Created by Ben on 2019-01-29.
//

#include "Content.h"

namespace mt {

	Content::Content(std::string name) : _name(name) {}
	std::string Content::name() const { return _name; }
	bool Content::inRange(size_t i) const { return  i == 0;}

}
