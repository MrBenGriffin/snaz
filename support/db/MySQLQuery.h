//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_MYSQLQUERY_H
#define MACROTEXT_MYSQLQUERY_H

#include <string>
#include "mysql.h"

#include "MySQLService.h"
#include "MySQLConnection.h"
#include "Query.h"

#include "support/Message.h"


namespace Support {
	namespace Db {

		class MySQLQuery : public Query {
		private:
			MySQLService* s;    //grandparent
			MySQLConnection* c; //parent
			MYSQL* queryHandle;	//copy of ConnectionHandle

			MYSQL_RES* result;
			MYSQL_ROW row;
			MYSQL_ROW_OFFSET start;
			MYSQL_ROW_OFFSET end;
			unsigned long* mysqlRowLengths;
			void prepareResult(Messages &);

			void rowRangeCheck(Messages &,size_t i);
			void fieldRangeCheck(Messages &,size_t i);
			void rowFieldRangeCheck(Messages &,size_t i, size_t j);

		private:
			friend class Query;
			friend class MySQLConnection;
			bool readfield(Messages &,size_t, std::string&) override;
			bool readfield(Messages &,size_t, long&)  override;		//
			bool readfield(Messages &,size_t, long double&)  override;		//
			bool readfield(Messages &,size_t, size_t, std::string&) override;

			MySQLQuery(MySQLService*, MySQLConnection*, MYSQL*&, std::string&);

			bool nextrow() override;
			void resetrows() override;

			void forQuery(Messages &,std::string& value) override; //convert a string to make ready for query..
			void readfieldforquery(Messages &,size_t i, size_t j, std::string& readString) override;
			void readFieldName(Messages &,size_t i, std::string&, std::string& ) override;

			void list_tables(Messages &,TableSet&) override;
			void listFields(Messages &,const std::string& table) override;

			bool execute(Messages &) override;
			void reset() override;

			~MySQLQuery() override;

		};

	}
}

#endif //MACROTEXT_MYSQLQUERY_H
