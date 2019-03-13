//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_CONNECTION_H
#define MACROTEXT_CONNECTION_H

#include <string>
#include <set>
#include <vector>
#include <unordered_map>
#include "support/Message.h"

namespace Support {
	namespace Db {
		using TableSet = std::set<std::string>;
		using FieldTypeMap = std::unordered_map<const string,string, std::hash<string> >;
		using TableFieldTypeMap = std::unordered_map<const string, FieldTypeMap, std::hash<string> >;
		using std::string;
		class Query;

		class Connection {

		private:
			TableSet table_set;
			TableFieldTypeMap tft_map;
			bool tset_loaded;

		public:
			bool table_exists(Messages&,const std::string&, bool = false);
			bool simple_result(Messages&,std::ostringstream&,std::vector<std::string>&);
			bool simple_query(Messages&,std::ostringstream&);
			bool exec(Messages&,const std::string&);
			bool field_type(Messages&,const std::string&, const std::string&, std::string&, bool=false);
			void lock(Messages&,std::string);
			void unlock(Messages&);
			virtual bool get_field_types(Messages&,const std::string&,FieldTypeMap&)=0;
			virtual bool open(Messages&,const std::string&)=0;
			virtual bool database(Messages&,const std::string&) = 0;
			virtual bool dbselected()=0; //if the database is selected we can instantiate queries.
			virtual bool primarykey(Messages&,const std::string&, std::set<std::string>&)=0; //
			virtual bool isopen()=0;
			virtual void close()=0;
			virtual bool last_insert_id(unsigned long long&)=0;
			virtual Query* query(Messages&,std::string = "")=0;
			virtual bool query(Messages&,Query*&,std::string = "")=0;
			virtual void escape(std::string&) =0;

			Connection();
			virtual ~Connection();
		};

	}
}

#endif //MACROTEXT_CONNECTION_H
