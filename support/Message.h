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
	enum channel { fatal, error, syntax, range, parms, warn, deprecated, info, debug, security, usage, link, trace, code, node, build, custom, path , container };
	enum Purpose { user, progress, timer, alert  };

	// Message is a single message.
	class Message {
	private:
		static std::atomic<uint64_t> master;
		uint64_t id;
		uint64_t parent;
		//		const Message* parent;
		bool synched;
	public:
		channel ch;
		Purpose purpose;
		string content;
		long double seconds;
		Message(channel,string);
		Message(channel,Purpose,string);
		Message(channel,Purpose,string, long double);
		string chan() const;
		string purp() const;
		uint64_t ID() const;
		void setParent(uint64_t);
		void str(ostream&) const;
		void store(Support::Messages&,Support::Db::Connection*);
	};

	// Messages are a deque of messages.
	class Messages {
	private:
		Messages* container;
		Support::Db::Connection* sql;
		uint64_t buildID;
		size_t userID;
		bool _marked;
		bool _established;
		deque<uint64_t> stack; //just the ID of the pushed alert.
		deque<Message> list;
	public:
		bool storing() { return sql != nullptr; }
		void startup();
		void synchronise();
		Messages(Support::Messages&);
		Messages(Support::Db::Connection*);
		~Messages();
		void setConnection(Support::Db::Connection*);
		bool marked() const;
		void reset();
		void push(const Message&);
		void add(const Message&);
		void pop();

		string line(size_t) const;
		uint64_t id() const { return buildID; }
		size_t   uid() const { return userID; }

		void operator<< ( const Message& );
		void operator+= ( Messages& );

		void str(ostream&, bool=false) const;
	};

}

#endif //MACROTEXT_MESSAGE_H
