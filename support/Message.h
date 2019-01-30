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

	enum channel { fatal, error, syntax, parms, warn, info, debug, scope, endsc, trace, code };

	// Message is a single message.
	class Message {
		size_t lineNum;
		size_t charNum;
	public:
		channel ch;
		string content;
		Message(channel,string);
		void str(ostream&);
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
		Messages& operator<< (Message);

		void str(ostream&);
	};


}
#endif //MACROTEXT_MESSAGE_H
