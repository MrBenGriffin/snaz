//
// Created by Ben Griffin on 2019-01-30.
//

#include "Message.h"
#include <ostream>

namespace Support {

	Message::Message(channel c,string s):  lineNum(SIZE_MAX),ch(c),content(s) {}

	void Message::str(ostream& log) {
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
		}
		log << ": ";
		if (lineNum != SIZE_MAX) {
			log << " at Line No." << lineNum << ", Character Pos. " << charNum << ". ";
		}
		log << content << endl;
	}

	string Messages::line(size_t line_number) const {
		if (_marked && (line_number > 0) && (line_number <= list.size())) {
			return list[line_number-1].content;
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
			}
		}
	}
}