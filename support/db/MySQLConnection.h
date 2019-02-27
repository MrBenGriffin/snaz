//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_MYSQLCONNECTION_H
#define MACROTEXT_MYSQLCONNECTION_H

#include <set>
#include <string>
#include "mysql.h"

#include "MySQLService.h"
#include "Connection.h"

#include "support/Message.h"

namespace Support {
	namespace Db {

		class MySQLConnection : public Connection {
		private:
			MySQLService *s;
			MYSQL *connectionHandle;
			bool conn_open;
			bool db_open;

		public:
			bool dbselected() override { return db_open && conn_open && connectionHandle != nullptr; }

			bool isopen() override { return conn_open && connectionHandle != nullptr; }

			bool open(Messages&,const std::string &) override; //use a file
			bool database(Messages&,const std::string &) override;

			bool primarykey(Messages&,const std::string &, std::set<std::string> &) override;

			bool get_field_types(Messages&,const std::string &, FieldTypeMap &) override;

			bool last_insert_id(unsigned long long &) override;

			Query *query(Messages&,std::string = "") override;

			bool query(Messages&,Query *&, std::string = "") override;

			void escape(std::string &) override;

			void close() override;

			explicit MySQLConnection(Messages&,MySQLService *);

			~MySQLConnection() override;
		};

	}
}

#endif //MACROTEXT_MYSQLCONNECTION_H
