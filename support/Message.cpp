#include <utility>

//
// Created by Ben Griffin on 2019-01-30.
//

#include <ostream>
#include <thread>
#include "support/Message.h"
#include "support/db/Connection.h"
#include "support/db/Query.h"
#include "Build.h"
#include "BuildUser.h"

namespace Support {

	std::atomic<uint64_t> Message::master = 1;

	Message::Message(channel c,string s): id(master++),ch(c),content(std::move(s)),parent(nullptr),synched(false) {
	}

	string Message::chan() const {
		switch (ch) { //syntax
			case security: 	return "security"; break;
			case fatal:  	return "fatal"; break;
			case error:  	return "error"; break;
			case syntax: 	return "syntax"; break;
			case parms:  	return "parms"; break;
			case range:  	return "range"; break;
			case warn:   	return "warn"; break;
			case info:   	return "info"; break;
			case debug:  	return "debug"; break;
			case usage:	 	return "usage"; break;
			case trace:  	return "trace"; break;
			case code:   	return "code"; break;
			case timing: 	return "timing"; break;
			case container: return "container"; break;
			case link:   	return "link"; break;
			case deprecated:return "deprecated"; break;
		}
	}

	void Message::str(ostream& log) const {
		log << chan() << ": " << content << flush;
	}

	void Message::store(Support::Messages& log,Support::Db::Connection* sql) {
		if(sql != nullptr && !synched) {
			Db::Query* query = nullptr;
			ostringstream item;
			string text(content);
			sql->escape(text);
			item << "insert ignore into bldlog (build,id,parent,user,channel,message) values (";
			item << log.id() << "," << id << ",";
			if (parent == nullptr) {
				item << "NULL";
			} else {
				item << parent->id;
			}
			size_t user = log.uid();
			if (user == 0) {
				item << ",NULL,'";
			} else {
				item << "," << user << ",'";
			}
			item << chan() << "','" << text << "')";
			if(sql->query(log,query,item.str())) {
				query->execute(log);
			}
			sql->dispose(query);
			synched = true;
		}
	}

	Messages::Messages(const Messages& o ): sql(o.sql),buildID(o.buildID),userID(o.userID),_established(o._established),_suppressed(false),_marked(false) {
	}

	Messages::Messages(Db::Connection* _sql): sql(_sql),buildID(0),userID(0),_established(false), _suppressed(false),_marked(false) {
		startup();
	}

	void Messages::setConnection(Db::Connection* _sql) {
		if(sql == nullptr) {
			sql = _sql;
			startup();
		}
	}
	void Messages::startup() {
		if(sql != nullptr && ! _established) {
			ostringstream table;
			table << "CREATE TABLE IF NOT EXISTS `bldlog` ("
			" `build` bigint unsigned not null,"
			"`id` bigint unsigned not null,"
			"`parent` bigint unsigned null,"
			"`user` int(11),"
			"`channel` enum ('fatal','error','syntax','range','parms','warn','deprecated','info','debug','security','usage','link','trace','code','timing','container','item') not null default 'item',"
			"`ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
			"`message` text NOT NULL,"
			"PRIMARY KEY (`build`,`id`),"
			"UNIQUE KEY (`id`,`parent`),KEY (`ts`),"
			"FOREIGN KEY (parent) REFERENCES bldlog(id) ON DELETE CASCADE, "
		 	"FOREIGN KEY (user) REFERENCES blduser(id) ON DELETE CASCADE) "
			"ENGINE=InnoDB DEFAULT CHARSET=utf8";
			Db::Query* query;
			if(sql->query(*this,query,table.str())) {
				query->execute(*this);
			}
			sql->dispose(query);
			if(sql->query(*this,query,"select uuid_short() as build") && query->execute(*this)) {
				while(query->nextrow()) {
					query->readfield(*this,"build",buildID);
				}
			}
			sql->dispose(query);
			userID = Build::b().user.iD();
			_established = true;
			synchronise();
		}
	}

	void Messages::synchronise() {
		userID = Build::b().user.iD();
		for (auto& m : list) {
			m.store(*this,sql);
		}
	}

	bool Messages::marked() const {
		return _marked;
	}
	bool Messages::suppressed() const {
		return _suppressed;
	}

	void Messages::reset() {
		synchronise();
		list.clear();
		_suppressed = false;
		_marked = false;
	}

	string Messages::line(size_t line_number) const {
		if ((line_number > 0) && (line_number <= list.size())) {
			ostringstream re;
			list[line_number-1].str(re);
			return re.str();
		}
		return "";
	}


	void Messages::contain(string text,Messages& contents) {
		list.push_back(Message(container,text));
		*this += contents;

	}

	void Messages::push(const Message& m) {
		list.push_back(std::move(m));
	}

	void Messages::pop() {

	}

	void Messages::prefix(Message m) {
		list.push_front(std::move(m));
	}

	void Messages::operator<< (const Message& m ) {
		if(!_suppressed) {
			_marked = _marked || (m.ch != info && m.ch != debug);
			list.push_back(std::move(m));
		}
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
