//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_MYSQLSERVICE_H
#define MACROTEXT_MYSQLSERVICE_H

#include <string>
#include "mysql.h"

#include "support/Message.h"
#include "Service.h"

#ifndef my_bool
#define my_bool bool
#endif

namespace Support {
	namespace Db {

		class MySQLService : public Service {
		private:
			void * zip_lib_handle;
			void * mysql_lib_handle;
			MYSQL* serviceHandle;		//for services only. Use a separate connectionHandle for connections.

		private:
			friend class MySQLDatabase;
			friend class MySQLConnection;
			friend class MySQLQuery;

//The mysql API that we use.
			int (*select_db)(MYSQL*, const char*);
			void (*close)(MYSQL*);
			void (*data_seek)(MYSQL_RES*,my_ulonglong);
			unsigned int (*my_errno)(MYSQL*);  //errno will be expanded as a macro from sys under some compilers.
			const char* (*error)(MYSQL*);
			MYSQL_FIELD* (*fetch_field_direct)(MYSQL_RES*,unsigned int);
			unsigned long* (*fetch_lengths)(MYSQL_RES*);
			MYSQL_ROW (*fetch_row)(MYSQL_RES*);
			void (*free_result)(MYSQL_RES*);
			MYSQL* (*init)(MYSQL*);
			my_ulonglong (*insert_id)(MYSQL*);
			MYSQL_RES* (*list_fields)(MYSQL*, const char*,const char*);
			MYSQL_RES* (*list_tables)(MYSQL*,const char*);
			unsigned int (*num_fields)(MYSQL_RES*);
			my_ulonglong (*num_rows)(MYSQL_RES*);
			int	(*options)(MYSQL *,enum mysql_option,const char *);
			MYSQL* (*real_connect)(MYSQL*, const char*,const char*,const char*,const char*, unsigned int,const char*, unsigned long);
			unsigned long (*real_escape_string)(MYSQL*,char*,const char*, unsigned long);
			int (*real_query)(MYSQL*, const char*, unsigned long);
			MYSQL_ROW_OFFSET (*row_seek)(MYSQL_RES*, MYSQL_ROW_OFFSET);
			MYSQL_ROW_OFFSET (*row_tell)(MYSQL_RES*);
			const char* (*character_set_name)(MYSQL *);
			int (*set_character_set)(MYSQL *, const char *);
			MYSQL_RES* (*store_result)(MYSQL*);
			MYSQL_STMT* (*mysql_stmt_init)(MYSQL*);
			my_bool* (*mysql_stmt_close)(MYSQL_STMT*);
			int (*mysql_stmt_execute)(MYSQL_STMT*);
			int (*mysql_stmt_prepare)(MYSQL_STMT*, const char*, unsigned long);

		public:
			explicit MySQLService(Messages&);
			virtual ~MySQLService();
			virtual Connection* instance(Messages&);

		};

	}
}

#endif //MACROTEXT_MYSQLSERVICE_H
