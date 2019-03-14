#include <utility>

//
// Created by Ben Griffin on 2019-01-30.
//

#include "Message.h"
#include <ostream>

namespace Support {

	Message::Message(channel c,string s): ch(c),content(std::move(s)) {}

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
			case even: 	 log << "-line"; break;
			case struc:  log << "....."; break;
			case link:   log << "link"; break;
		}
		log << ": " << content << flush;
	}


	Messages::format Messages::Format = Messages::Html;
	size_t Messages::Verbosity = 3;
	bool Messages::Deferred = true;

	Messages::Messages(): _suppressed(false),_marked(false) {
//		list.clear();
	}

	void Messages::setVerbosity(size_t v) {
		Verbosity = v;
	}
	void Messages::setMarkup(bool useMarkup) {
		Format =  useMarkup ? Html : Console;
	}
	void Messages::defer(bool defer) {
		Deferred = defer;
	}

	bool Messages::marked() const {
		return _marked;
	}
	bool Messages::suppressed() const {
		return _suppressed;
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

	void Messages::operator<< (const Message& m ) {
		_marked = _marked || (m.ch != info && m.ch != debug);
		list.push_back(std::move(m));
//		return *this;
	}

	void Messages::operator+= (Messages& msgs) {
		_marked = _marked || msgs._marked;
		while (! msgs.list.empty() ) {
			list.push_back(std::move(msgs.list.front()));
			msgs.list.pop_front();
		}
	}

	void Messages::suppress(bool supp) {
		_suppressed=supp;
	}

	void Messages::str(ostream& o, bool anyway) const {
		if (_marked || anyway) {
			for (auto& m : list) {
				m.str(o);
				o << endl;
			}
		}
		o << flush;
	}
}