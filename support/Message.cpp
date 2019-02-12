//
// Created by Ben Griffin on 2019-01-30.
//

#include "Message.h"
#include <ostream>

namespace Support {

	Message::Message(channel c,string s): ch(c),content(s) {}

	void Message::str(ostream& log) const {
		switch (ch) { //syntax
			case fatal:  log << "fatal"; break;
			case error:  log << "error"; break;
			case syntax: log << "syntax"; break;
			case parms:  log << "parms"; break;
			case range:  log << "range"; break;
			case warn:   log << "warn"; break;
			case info:   log << "info"; break;
			case debug:  log << "debug"; break;
			case scope:  log << "<<<<<"; break;
			case endsc:  log << ">>>>>"; break;
			case usage:	 log << "using"; break;
			case trace:  log << "trace"; break;
			case code:   log << "-raw-"; break;
			case timing: log << "time"; break;
		}
		log << ": " << content << flush;
	}

	string Messages::line(size_t line_number) const {
		if ((line_number > 0) && (line_number <= list.size())) {
			ostringstream re;
			list[line_number-1].str(re);
			return re.str();
		}
		return "";
	}

	void Messages::enscope(string s) {
		list.push_front(std::move(Message(scope,s)));
		list.push_back(std::move(Message(endsc,"")));
	}

	void Messages::prefix(Message m) {
		list.push_front(std::move(m));
	}

	Messages& Messages::operator<< (Message m) {
		_marked = _marked || (m.ch != info && m.ch != debug);
		list.push_back(std::move(m));
		return *this;
	}

	void Messages::str(ostream& o) {
		if (_marked) {
			for (auto& m : list) {
				m.str(o);
				o << endl;
			}
		}
	}
}