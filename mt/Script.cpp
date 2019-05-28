//
// Created by Ben on 2019-05-15.
//

#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"
#include "Script.h"

namespace mt {

	Script::Script(std::string w) : Token(),text(std::move(w)) {}

	bool Script::empty() const {
		return text.empty();
	}

	void Script::final(std::ostream &o) const {
		o << text;
	}

	std::string Script::get() const {
		return text;
	}

	void Script::append(std::string right) {
		text.append(std::move(right));
	}
}