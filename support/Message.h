//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_MESSAGE_H
#define MACROTEXT_MESSAGE_H

#include <utility>
#include <sstream>
#include <deque>
#include <ostream>

namespace Support {
	using namespace std;

	enum channel { fatal, error, syntax, range, parms, warn, info, debug, usage, scope, endsc, trace, code, timing };

	// Message is a single message.
	class Message {
	public:
		channel ch;
		string content;
		Message(channel,string);
		void str(ostream&) const;
	};

	// Messages are a deque of messages.
	class Messages {
	private:
		bool _suppressed;
		bool _marked;
		deque<Message> list;
	public:
		Messages() : _suppressed(false),_marked(false) {}
		bool marked() const { return _marked; }
		bool suppressed() const { return _suppressed; }
		void suppress(bool s=true) { _suppressed=s; }
		void prefix(Message);
		void enscope(string);
		string line(size_t) const;

		Messages& operator<< (Message);

		void str(ostream&);
	};


}
#endif //MACROTEXT_MESSAGE_H
