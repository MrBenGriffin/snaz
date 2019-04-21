//
// Created by Ben on 2019-02-27.
//

#include <sstream>
#include <string>
#include <set>
#include "support/Env.h"

#include "Service.h"
#include "Connection.h"
#include "Query.h"


namespace Support {
	namespace Db {
		Connection::Connection() : table_set(),tft_map() {
		}

		Connection::~Connection() {
			table_set.clear();
			tft_map.clear();
		}

		void Connection::lock(Messages& errs,string tablelist) {
			Query* q= nullptr;
			string locker = "lock tables " + tablelist;
			if (query(errs, q,locker)) {
				if (! q->execute(errs) ) {
					errs << Message(error,locker);
				}
				delete q;
			}
		}

		void Connection::unlock(Messages& errs) {
			Query* q= nullptr;
			if (query(errs, q,"unlock tables")) {
				if (! q->execute(errs) ) {
					errs << Message(error,"unlock tables");
				}
				delete q;
			}
		}

		bool Connection::exec(Messages& errs,const std::string& querystr) {
			bool retval = false;
			Query *q = nullptr;
			if ( query(errs, q,querystr) ) {
				if ( q->execute(errs) ) { retval = true; }
				delete q;
			}
			return retval;
		}

		//one may set refresh to true, if it is necessary to find tables that have been
		//created since the start of the process.
		bool Connection::table_exists(Messages& errs,const std::string& table_name, bool refresh) {
			bool retval = false;
			if (dbselected()) {
				if(refresh) { table_set.clear(); }
				if ( table_set.empty()  ) {
					Query* q= nullptr;
					if (query(errs, q,"show tables")) {
						q->list_tables(errs,table_set);
						dispose(q);
					}
				}
				if (table_set.find(table_name) != table_set.end()) {
					retval = true;
				}
			}
			return retval;
		}

		bool Connection::field_type(Messages& errs,const std::string& table_name, const std::string& field_name, std::string& result, bool refresh) {
			bool retval = false;
			if (dbselected() && table_exists(errs,table_name,refresh) ) {
				TableFieldTypeMap::const_iterator tft_i = tft_map.find(table_name);
				if ( !refresh && (tft_i != tft_map.end()) ) {
					const FieldTypeMap& ltft_map = tft_i->second;
					auto ft_i = ltft_map.find(field_name);
					if ( ft_i != ltft_map.end()) {
						result=ft_i->second;
						retval = true;
					} else {
						retval = false; //field is not there.
					}
				} else { //table is there but we refresh, or table isn't there.
					FieldTypeMap f_map;
					if (get_field_types(errs,table_name,f_map)) {
						pair<TableFieldTypeMap::iterator, bool> ins = tft_map.insert(TableFieldTypeMap::value_type(table_name, f_map));
						if (!ins.second)	{
							tft_map.erase(ins.first);
							tft_map.insert(TableFieldTypeMap::value_type(table_name, f_map));
						}
						FieldTypeMap::const_iterator ft_i = f_map.find(field_name);
						if ( ft_i != f_map.end()) {
							result=ft_i->second;
							retval = true;
						} else {
							retval = false; //field is not there.
						}
					}
				}
			}
			return retval;
		}
	}
}