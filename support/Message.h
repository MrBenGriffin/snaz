//
// Created by Ben Griffin on 2019-01-30.
//

#ifndef MACROTEXT_MESSAGE_H
#define MACROTEXT_MESSAGE_H

#include <utility>
#include <sstream>
#include <deque>
#include <ostream>
#include <cstdint>

namespace Support {
	namespace Db {
		class Connection;
	}
	class Message;
	class Messages;

	using namespace std;
// 'item' is used in the database for undefined...
	enum channel { fatal, error, syntax, range, parms, warn, deprecated, info, debug, security, usage, link, trace, code, timing, container };

	// Message is a single message.
	class Message {
	private:
		static std::atomic<uint64_t> master;
		uint64_t id;
		const Message* parent;
		uint64_t parentID();
		bool synched;
	public:
		channel ch;
		string content;
		Message(channel,string);
		string chan() const;
		void str(ostream&) const;
		void store(Support::Messages&,Support::Db::Connection*);
	};

	// Messages are a deque of messages.
	class Messages {
	private:
		Support::Db::Connection* sql;
		uint64_t buildID;
		size_t userID;
//		enum format { Console, Html, Sql };
		bool _suppressed;
		bool _marked;
		bool _established;
		deque<Message> stack;
		deque<Message> list;
	public:
		void startup();
		void synchronise();
		Messages(const Support::Messages&);
		Messages(Support::Db::Connection*);
		void setConnection(Support::Db::Connection*);
		bool marked() const;
		void reset();
		bool suppressed() const;
		void suppress(bool=false);
		void prefix(Message);
		void push(const Message&);
		void pop();

		void contain(string,Messages&); //quick push/pop.

		string line(size_t) const;
		uint64_t id() const { return buildID; }
		size_t   uid() const { return userID; }

		void operator<< (const Message& );
		void operator+= ( Messages& );

		void str(ostream&, bool=false) const;
	};

}

#endif //MACROTEXT_MESSAGE_H
