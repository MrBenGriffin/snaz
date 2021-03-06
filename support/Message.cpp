#include <utility>

//
// Created by Ben Griffin on 2019-01-30.
//

#include <iostream>
#include <ostream>
#include <thread>
#include "support/Message.h"
#include "support/Fandr.h"
#include "support/db/Connection.h"
#include "support/db/Query.h"
#include "Build.h"
#include "BuildUser.h"

namespace Support {

	std::atomic<uint64_t> Message::master = 1;
	Message::Message(channel c, Purpose p,string s, long double t) :
		id(master++),ch(c),purpose(p),content(std::move(s)),parent(0),synched(false),prog(t),seconds(t) {
	}

	Message::Message(channel c, string s, long double t) :  Message(c,extra,std::move(s),0.0L) {
		switch(c) {
			case build:
			case ntime:
			case custom:
				purpose=timer;
				seconds=t;
				break;
			case Support::node:
			case Support::language:
			case Support::technology:
			case Support::media:
			case Support::transform:
			case Support::adhoc:
			case Support::file:
				purpose=progress;
				prog = t;
				break;
			default:
				prog = t;
				break;
		}
	}

	Message::Message(channel c, Purpose p,string s) : Message(c,p,std::move(s),0.0L) {
	}

	Message::Message(channel c, string s,const mt::location& l,string addon) : Message(c,alert,std::move(s),0.0L) {
		setPurpose();
		loc = l;
		additional=addon;
	}

	Message::Message(channel c,string s): Message(c,alert,std::move(s),0.0L) {
		setPurpose();
	}

void Message::setPurpose() {
	switch (ch) { //syntax
		case security:
		case fatal:
		case error:
		case syntax:
		case undefined:
		case parms:
		case range:
			purpose = alert; break;
			
		case warn:
		case deprecated:
		case usage:
		case path:
			purpose = notice; break;

		case container:
		case trace:
		case info:
		case debug:
		case code:
		case link:
			purpose = extra; break;

		case channel::adhoc:
		case channel::file:
		case channel::node:
		case channel::language:
		case channel::technology:
		case channel::media:
		case channel::transform:
			purpose = progress; break;

		case ntime:
		case build:
		case custom:
			purpose = timer; break;
	}

}

	void Message::setParent(uint64_t par) {
		parent=par;
	}

	uint64_t Message::ID() const {
		return id;
	}

	string Message::purposeStr() const {
		switch (purpose) {
			case progress:
				return "progress";
			case user:
				return "user";
			case timer:
				return "timer";
			case alert:
				return "alert";
			case notice:
				return "notice";
			case extra:
				return "extra";
			default:
				return "item"; //should never reach here.
		}
	}

	string Message::chan() const {
		switch (ch) { //syntax
			case security: 	return "security"; break;
			case fatal:  	return "fatal"; break;
			case error:  	return "error"; break;
			case syntax: 	return "syntax"; break;
			case undefined: return "undefined"; break;
			case parms:  	return "parms"; break;
			case range:  	return "range"; break;
			case warn:   	return "warn"; break;
			case info:   	return "info"; break;
			case debug:  	return "debug"; break;
			case usage:	 	return "usage"; break;
			case trace:  	return "trace"; break;
			case code:   	return "code"; break;
			case ntime: 	return "ntime"; break;
			case build: 	return "build"; break;
			case custom: 	return "custom"; break;
			case path: 		return "path"; break;
			case container: return "container"; break;
			case link:   	return "link"; break;
			case deprecated:return "deprecated"; break;
			case channel::node: return "node"; break;
			case channel::language: return "language"; break;
			case channel::technology: return "technology"; break;
			case channel::media: return "media"; break;
			case channel::transform: return "transform"; break;
			case channel::adhoc: return "adhoc"; break;
			case channel::file: return "file"; break;
		}
		return "item";
	}

	void Message::str(ostream& log) const {
		log << purposeStr() << "; " << chan() << ": " << content;
	}

	void Message::json(ostream& o) const {
		string x(content);
		Support::fandr(x, "\"", "\\\"");
		o << "\t\t\t{" << endl;
		o << "\t\t\t\t\"channel\": \"" << chan() << "\"," << endl;
		o << "\t\t\t\t\"message\": \"" << content << "\"";
		if(!additional.empty()) {
			o << "," << endl << "\t\t\t\t\"example\": \"" << additional << "\"";
		}
		if (loc.end.column - loc.begin.column > 0) {
			o << "," << endl;
			o << "\t\t\t\t\"begin\": {" << endl;
			o << "\t\t\t\t\t\"col\": " << loc.begin.column << "," << endl;
			o << "\t\t\t\t\t\"row\": " << loc.begin.line << endl;
			o << "\t\t\t\t}," << endl;
			o << "\t\t\t\t\"end\": {" << endl;
			o << "\t\t\t\t\t\"col\": " << loc.end.column - 1 << "," << endl;
			o << "\t\t\t\t\t\"row\": " << loc.end.line << endl;
			o << "\t\t\t\t}" << endl;
		} else {
			o << endl;
		}
		o << "\t\t\t}";
	}

	void Message::store(Support::Messages& log,Support::Db::Connection* sql) {
		if(sql != nullptr && !synched) {
			Db::Query* query = nullptr;
			ostringstream item;
			string text(content);
			sql->escape(text);
			item << "insert ignore into bldlog (build,id,parent,user,purpose,channel,message,seconds,progress) values (";
			item << log.id() << "," << id << ",";
			if (parent == 0) {
				item << "NULL";
			} else {
				item << parent;
			}
			size_t user = log.uid();
			if (user == 0) {
				item << ",NULL,";
			} else {
				item << "," << user << ",";
			}
			item << "'" << purposeStr() << "','" << chan() << "','" << text << "',";
			if (seconds == 0) {
				item << "NULL,";
			} else {
				item << seconds << ",";
			}
			if (purpose != progress ) {
				item << "NULL";
			} else {
				item << prog ;
			}
			item << ")";
			if(sql->query(log,query,item.str())) {
				if(!query->execute(log)) {
					log.str(std::cerr);
					log.reset();
				}
			}
			sql->dispose(query);
			synched = true;
		}
	}

	void Messages::setProgressSize(Message::level lev,size_t count) {
		switch (lev) {
			case Message::technology: progressSize[0] = count; break;
			case Message::language: progressSize[1] = count; break;
			case Message::node: progressSize[2] = count; break;
			case Message::media: progressSize[4] = count; break;
			case Message::transform: progressSize[5] = count; break;
		}
	}

	void Messages::calculateProgressSize() {
		progressAll = 1.0L;
		for(size_t i=0; i<3; i++) {
			// Calculate node progress as tech*language*nodes
			// Templates are calculated as a fraction of each node.
			progressAll *= progressSize[i];
		}
		//Adhocs, Media and transforms are not nested, so are just added.
		progressAll += progressSize[3]; // Adhocs (currently there are 2).
		progressAll += progressSize[4]; // Media. Transforms are done (fractionally) over Media, as are templates.
	}

	Messages::Messages(Messages& o ): sql(o.sql),buildID(o.buildID),userID(o.userID),_established(o._established),_marked(false),_debug(o._debug) {
		container = &o;
	}

	Messages::Messages(Db::Connection* _sql): container(nullptr),sql(_sql),buildID(0),userID(0),_established(false),_marked(false),_debug(true) {
		startup();
	}

	Messages::~Messages() {
		if(container == nullptr) {
			synchronise();
		} else {
			while (! list.empty() ) {
				Message& item(list.front());
				container->list.emplace_back(std::move(item));
				list.pop_front();
			}
		}
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
			"`seconds` double null,"
			"`progress` double null,"		// stored between 0..1
			"`purpose` enum ('progress','user','timer','alert','notice','extra') not null default 'alert',"
			"`channel` enum ("
   			"'fatal','error','syntax','range','parms','warn','undefined','deprecated','info','debug','security','usage'," //alerts
	  		"'path','link','trace','code','container'," 								// extras
	 		"'ntime','build','custom',"													// timing
			"'node','language','technology','media','transform','adhoc','file',"		// progress
			"'item') not null default 'item',"
			"`ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
			"`message` text NOT NULL,"
			"PRIMARY KEY (`build`,`id`),"
			"KEY (`build`,`id`,`parent`),KEY (`ts`),"
			"FOREIGN KEY (build,parent) REFERENCES bldlog(build,id) ON DELETE CASCADE, "
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

	void Messages::shutdown() {
		if(sql != nullptr &&  _established) {
			synchronise();
		}
	}


	void Messages::synchronise() {
		for (auto& m : list) {
			m.store(*this,sql);
		}
	}

	bool Messages::marked() const {
		return _marked;
	}

	bool Messages::justMarked() const {
		if (_marked && !list.empty()) {
			auto& m = list.back();
			return m.purpose == alert;
		} else return false;
	}

	void Messages::reset() {
		list.clear();
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

	void Messages::add(Message const& m) {
		if (_debug || m.ch != debug) {
			_marked = _marked || (m.purpose == alert);
			long double progression(0L);
			if (m.purpose == progress) {
				switch (m.ch) {
					case media:    //ONLY USE THIS IF NOT USING Transforms FOR THE CURRENT Media.
					case transform:
					case file:
						currentProgress += m.prog;
						break;  //fractionals.

					case node: //ONLY USE THIS IF NOT USING FILE FOR THE CURRENT NODE.
					case adhoc:
						currentProgress++;
						break;
					default:
						break; //only the above mean anything for progress.
				}
				progression = currentProgress / progressAll;
			}
			list.emplace_back(std::move(m));
			if (!stack.empty()) {
				list.back().setParent(stack.back());
			}
			if (progression != 0L) {
				list.back().prog = progression;
				synchronise();
			}
			if (m.ch == debug) {
				synchronise();
			}
		}
	}

	void Messages::push(Message const& m) {
		auto id = m.ID();
		add(m);
		stack.push_back(id);
		synchronise();
			// We have to push the stack message in order to ensure that we get referential integrity on parent.
	}

	void Messages::pop() {
		auto top = stack.back();
		auto curr = list.back().ID();
		if (top == curr) {
			list.pop_back();
		}
		stack.pop_back();
	}

	size_t Messages::uid() {
		if(userID == 0)
			userID = Build::b().user.iD();
		return userID;
	}

	void Messages::operator<< (Message const& m) {
		add(m);
	}

	void Messages::operator+= (Messages& msgs) {
		_marked = _marked || msgs._marked;
		while (! msgs.list.empty() ) {
			auto* msg = &(msgs.list.front());
			if(!msg->synched) {
				list.emplace_back(std::move(msgs.list.front()));
			}
			msgs.list.pop_front();
		}
	}

	void Messages::json(ostream& o) const {
		o << "{" << endl << "\t\"data\":{" << endl;
		o << "\t\t\"passed\":" << (_marked ? "false" : "true")
			<< "," << endl << "\t\t\"errors\": [" << endl;
		for (auto& m : list) {
			m.json(o);
			if (&m != &list.back()) {
				o << "," ;
			}
			o << endl;
		}
		o << "\t\t]" << endl << "\t}" << endl << "}" << endl << flush;
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
