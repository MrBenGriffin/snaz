//
// Created by Ben on 2019-02-27.
//
#include <iostream>

#include "mysql.h"
#include "errmsg.h"

#include "MySQLConnection.h"
#include "Query.h"
#include "MySQLQuery.h"

namespace Support {
	namespace Db {


			MySQLConnection::MySQLConnection(Messages& errs,MySQLService* service) : s(service),connectionHandle(nullptr),conn_open(false),db_open(false) {
				connectionHandle = s->init(nullptr);
				if (connectionHandle == nullptr) {
					errs << Message(fatal,"MySQLConnection error:: Failed to initialise a MySQL client connection handle");
				}
			};

			MySQLConnection::~MySQLConnection() {
				if (connectionHandle != nullptr) {
					s->close(connectionHandle);
				}
			}

			bool MySQLConnection::last_insert_id(unsigned long long& idcontainer) {
				bool retval = false;
				if ( isopen() ) {
					my_ulonglong idcont = s->insert_id(connectionHandle);
//			if ( idcont < 0xFFFF ) {
					idcontainer = (int)idcont;
//			} else {
//				idcontainer = 0;
//			}
					retval = idcontainer == 0 ? false : true;
				}
				return retval;
			}

			bool MySQLConnection::primarykey(Messages& errs,const std::string& table, std::set<std::string>& primary) {
				bool retval = false;
				if (  isopen() && table_exists(errs,table) ) { //should we refresh the table list here? best not to.
					std::ostringstream qstr;
					qstr << "desc " << table;
					Query* q = query(errs,qstr.str());
					if (q->execute(errs)) {
						std::string name,keytype;
						while(q->nextrow()) {
							q->readfield(errs,"Key",keytype);
							if (keytype == "PRI") {
								q->readfield(errs,"Field",name);
								primary.insert(name);     //name field
							}
						}
					}
					delete q;
				}
				return retval;
			}

			bool MySQLConnection::get_field_types(Messages& errs,const std::string& table,FieldTypeMap& result) {
				bool retval = false;
				if (  isopen() && table_exists(errs,table) ) { //should we refresh the table list here? best not to.
					std::ostringstream qstr;
					qstr << "desc " << table;
					Query* q = query(errs,qstr.str());
					if (q->execute(errs)) {
						std::string f_name,f_type;
						while(q->nextrow()) {
							q->readfield(errs,"Field",f_name);
							q->readfield(errs,"Type",f_type);
							pair<FieldTypeMap::iterator, bool> ins = result.insert(FieldTypeMap::value_type(f_name, f_type));
							if (!ins.second)	{
								result.erase(ins.first); result.insert(FieldTypeMap::value_type(f_name, f_type));
							}
						}
						retval = true;
					}
					delete q;
				}
				return retval;
			}

			bool MySQLConnection::open(Messages& errs,const std::string& file) { //use a config file
				if ( ! isopen() ) {
					if (s->options(connectionHandle,MYSQL_READ_DEFAULT_FILE,file.c_str()) == 0) {
						s->options(connectionHandle,MYSQL_READ_DEFAULT_GROUP,"mysql");
						if (s->real_connect(connectionHandle, nullptr, nullptr, nullptr, NULL, 0, nullptr, 0) == nullptr) {
							string errorMessage = s->error(connectionHandle);
							errs << Message(fatal,"MySQLConnection error:: Connection failed with '" + errorMessage + "'");
							conn_open = false;
							db_open = false;
						} else {
							conn_open = true;
							db_open = true; //not really?!
						}
					} else {
						errs << Message(fatal,"MySQLConnection error: Possibly file '" + file + " is not valid or not found.");
					}
				} else {
					errs << Message(error,"MySQLConnection error: Each connection can only be opened once. Instantiate a new connection.");
				}
				return isopen();
			}


			bool MySQLConnection::database(Messages& errs,const std::string& db) {
				if ( isopen() ) {
					int success = s->select_db(connectionHandle, db.c_str());
					if ( success != 0) {
						int err = s->my_errno(connectionHandle);
						if (err == CR_SERVER_GONE_ERROR || err == CR_SERVER_LOST) {
							close();
						}
						string errstr = "MySQLConnection error: While attempting to open database: ";
						errs << Message(fatal, errstr + s->error(connectionHandle));
						return false;
					} else {
						db_open = true;
						return true;
					}
				} else {
					errs << Message(fatal,"MySQLConnection error: Open a connection before selecting a database!");
					return false;
				}
			}

			bool MySQLConnection::query(Messages& errs,Query*& q,std::string query_str) {
				bool retval = false; q=nullptr;
				if ( isopen() ) {
//			if ( q != nullptr ) delete q; -- must be deleted by containing fn.
					if ( db_open ) {
						q = new MySQLQuery(s, this, connectionHandle, query_str);
						retval = true;
					} else {
						errs << Message(fatal,"MySQLConnection error: select a database before creating a query!");
						q = nullptr;
					}
				} else {
					errs << Message(fatal,"MySQLConnection error: Open a connection before creating a query!");
					q = nullptr;
				}
				return retval;
			}

			Query* MySQLConnection::query(Messages& errs,std::string query_str) {
				if ( isopen() ) {
					if ( db_open ) {
						return new MySQLQuery(s, this, connectionHandle, query_str);
					} else {
						errs << Message(fatal,"MySQLConnection error: select a database before creating a query!");
						return nullptr;
					}
				} else {
					errs << Message(fatal,"MySQLConnection error: Open a connection before creating a query!");
					return nullptr;
				}
			}

//	"Note that MYSQL* must be a valid, open connection. This is needed because the escaping depends on the character set in use by the server."
			void MySQLConnection::escape(std::string& text) {
				if ( ! text.empty() && 	conn_open && connectionHandle != nullptr) {
					auto * tbuf = new char[text.length()*2+1];
					long size = s->real_escape_string(connectionHandle, tbuf, text.c_str(), text.length());
					std::string escapedString(tbuf, size);
					delete [] tbuf;
					text = escapedString;
				}
			}

			void MySQLConnection::close() {
				if (conn_open && connectionHandle != nullptr) {
					s->close(connectionHandle);
					connectionHandle = nullptr;
					conn_open = false;
					db_open = false;
				}
			}

	}
}