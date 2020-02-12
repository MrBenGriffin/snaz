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
#include <atomic>
#include "location.hh"

namespace Support {
	namespace Db {
		class Connection;
	}
	class Message;
	class Messages;

	using namespace std;
// 'item' is used in the database for undefined...
	enum channel {
			fatal, error, syntax, range, parms, undefined, warn, deprecated, info, debug, security, usage, //alerts
			path, container, link, trace, code,  // extra
			ntime, build, custom,		   // timing
			node, language, technology, media, transform, adhoc, file     // progress
			};
	enum Purpose { user, progress, timer, alert, notice, extra };

	class Message {
/**
 * each message represents a logline.
 * It (normally) has a parent.
 * It also has both a channel and a purpose.
 * Purposes affect the information contained within it.
 * Progress messages should have a value and an aggregate, and represent eg 1/2 etc.
 * Importantly, there's a multiplicative component such that children of parents will derive their ancestry.
 * So, imagine there's 30 nodes, 20 Media, 2 languages, and 2 technologies..
 * 1/1 Initialise
 * 1/2T,
 * 		1/2L,
 * 			1/30N ...30/30N,
 * 			1/20M ...20/20M
 * 		2/2L,
 * 			1/30N ...30/30N,
 * 			1/20M ...20/20M
 * 2/2T,
 * 		1/2L,
 * 			1/30N ...30/30N,
 * 			1/20M ...20/20M
 * 		2/2L,
 * 			1/30N ...30/30N
 * 			1/20M ...20/20M
 * 1/1 Close
 * By having a progress stack in messages, we can manage the local progress and the global progress..
 * We could also have templates added to the mix...
 * The
 */
	public:
		enum level { technology=0, language=1, node=2, media=4, transform=5 };

		bool synched;
	private:
		static std::atomic<uint64_t> master;
		uint64_t id;
		uint64_t parent;
		mt::location loc;

		Message(channel,Purpose,string,long double);
		void setPurpose();
//		Message(const Message&) = delete;
	public:
		channel ch;
		Purpose purpose;
		string content;
		string additional;
		long double prog; //current progress out of
		long double seconds;
		Message(channel,string);
		Message(channel,Purpose,string);
		Message(channel,string,const mt::location&,string="");
		Message(channel,string,long double);
		string chan() const;
		string purposeStr() const;
		uint64_t ID() const;
		void setParent(uint64_t);
		void str(ostream&) const;
		void json(ostream&) const;
		void store(Support::Messages&,Support::Db::Connection*);
	};

	class Messages {
	private:
		Messages* container;
		Support::Db::Connection* sql;
		uint64_t buildID{0};
		size_t userID{0};
		bool _debug{true};
		bool _marked{false};
		bool _established{false};
		deque<uint64_t> stack; //just the ID of the pushed alert.
		deque<Message> list;
		//                          T L N A Mx Mt
		size_t progressSize [6] = { 1,1,1,2,1,1 };  //This must be filled before progress can be recorded.
		long double progressAll     = {1.0}; //eg 360
		long double currentProgress = {0.0}; //eg 212

	public:
		void calculateProgressSize();
		void setProgressSize(Message::level,size_t);

		bool storing() { return sql != nullptr; }
		void startup();
		void set_debug(bool debug=false) { _debug = debug;}
		void set_no_store() { sql = nullptr; }

		void shutdown();
		void synchronise();
		Messages(Support::Messages&);
		Messages(Support::Db::Connection*);
		~Messages();
		void setConnection(Support::Db::Connection*);
		bool marked() const;
		void reset();
		void push(Message const&);
		void add(Message const&);
		bool justMarked() const;
		void pop();

		string line(size_t) const;
		uint64_t id() const { return buildID; }
		size_t   uid();

		void operator<< ( Message const& );
		void operator+= ( Messages& );

		void str(ostream&, bool=false) const;
		void json(ostream&) const;

	};

}

#endif //MACROTEXT_MESSAGE_H
