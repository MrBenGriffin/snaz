//
// Created by Ben on 2019-05-15.
//

#include "support/Message.h"
#include "support/Convert.h"
#include "support/Fandr.h"
#include "mt.h"
#include "Script.h"

namespace mt {

	Script::Script(const std::string& w) : Token(),size(0) {
		text << w;
		size = text.tellp();
	}

	Script::Script(const std::ostringstream& o) : Token(),size(0) {
		text << o.str();
		size = text.tellp();
	}

	bool Script::empty() const {
		return size == 0;
	}

	void Script::final(std::ostream &o) const {
		o << text.str() ;
	}

	std::string Script::get() const {
		return text.str();
	}
		
	void Script::append(const std::ostringstream& right) {
			text << right.str();
			size += text.tellp();
	}

	void Script::append(const std::string& right) {
		text << right;
		size += text.tellp();
	}
		
	void Script::inject(Messages&,mtext& out,mstack&) const {
			Token::add(this,out);
	}

}
