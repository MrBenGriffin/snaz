//
// Created by Ben on 2019-02-27.
//
#include <dlfcn.h>

#include "MySQLService.h"
#include "MySQLConnection.h"
#include "support/Env.h"
#include "support/Dynamic.h"

namespace Support {
	namespace Db {
		using namespace std;
		MySQLService::MySQLService(Messages &errs) :
				Service(), zip_lib_handle(nullptr), mysql_lib_handle(nullptr), serviceHandle(nullptr),
				select_db(nullptr), close(nullptr), data_seek(nullptr), my_errno(nullptr), error(nullptr),
				fetch_field_direct(nullptr),
				fetch_lengths(nullptr), fetch_row(nullptr), free_result(nullptr), init(nullptr), insert_id(nullptr),
				list_fields(nullptr),
				num_fields(nullptr), num_rows(nullptr), options(nullptr), real_connect(nullptr),
				real_escape_string(nullptr),
				mysql_stmt_init(nullptr),mysql_stmt_close(nullptr),mysql_stmt_execute(nullptr),mysql_stmt_prepare(nullptr),
				real_query(nullptr), row_seek(nullptr), row_tell(nullptr), set_character_set(nullptr),
				character_set_name(nullptr), store_result(nullptr) {
			_service = false;
			string mysqllib, ziplib;
			Env& env = Env::e();
			bool err = false; //necessary IFF script uses pcre.
			if (!env.get("LIBZIPSO", ziplib)) ziplib = SO(libz);
			if (!env.get("LIBMYSQLCRSO", mysqllib)) mysqllib = SO(libmysqlclient);

			zip_lib_handle = dlopen(ziplib.c_str(),RTLD_LAZY | RTLD_GLOBAL);  //dlopen MUST have one of RTLD_LAZY,RTLD_NOW
			err = dlerr(errs);
			if (!err && zip_lib_handle != nullptr) {
				mysql_lib_handle = dlopen(mysqllib.c_str(), RTLD_LAZY);
				err = dlerr(errs);
				if (mysql_lib_handle != nullptr) {
					if (!err) {
						select_db = (int (*)(MYSQL *, const char *)) dlsym(mysql_lib_handle, "mysql_select_db");
						err = dlerr(errs);
						if (!err) {
							close = (void (*)(MYSQL *)) dlsym(mysql_lib_handle, "mysql_close");
							err = dlerr(errs);
						}
						if (!err) {
							data_seek = (void (*)(MYSQL_RES *, my_ulonglong)) dlsym(mysql_lib_handle,
																					"mysql_data_seek");
							err = dlerr(errs);
						}
						if (!err) {
							my_errno = (unsigned int (*)(MYSQL *)) dlsym(mysql_lib_handle, "mysql_errno");
							err = dlerr(errs);
						}
						if (!err) {
							error = (const char *(*)(MYSQL *)) dlsym(mysql_lib_handle, "mysql_error");
							err = dlerr(errs);
						}
						if (!err) {
							fetch_field_direct = (MYSQL_FIELD *(*)(MYSQL_RES *, unsigned int)) dlsym(mysql_lib_handle,
																									 "mysql_fetch_field_direct");
							err = dlerr(errs);
						}
						if (!err) {
							fetch_lengths = (unsigned long *(*)(MYSQL_RES *)) dlsym(mysql_lib_handle,
																					"mysql_fetch_lengths");
							err = dlerr(errs);
						}
						if (!err) {
							fetch_row = (MYSQL_ROW (*)(MYSQL_RES *)) dlsym(mysql_lib_handle, "mysql_fetch_row");
							err = dlerr(errs);
						}
						if (!err) {
							free_result = (void (*)(MYSQL_RES *)) dlsym(mysql_lib_handle, "mysql_free_result");
							err = dlerr(errs);
						}
						if (!err) {
							init = (MYSQL *(*)(MYSQL *)) dlsym(mysql_lib_handle, "mysql_init");
							err = dlerr(errs);
						}
						if (!err) {
							insert_id = (my_ulonglong (*)(MYSQL *)) dlsym(mysql_lib_handle, "mysql_insert_id");
							err = dlerr(errs);
						}
						if (!err) {
							list_fields = (MYSQL_RES *(*)(MYSQL *, const char *, const char *)) dlsym(mysql_lib_handle,
																									  "mysql_list_fields");
							err = dlerr(errs);
						}
//						if (!err) {
//							list_tables = (MYSQL_RES *(*)(MYSQL *, const char *)) dlsym(mysql_lib_handle,
//																						"mysql_list_tables");
//							err = dlerr(errs);
//						}
						if (!err) {
							num_fields = (unsigned int (*)(MYSQL_RES *)) dlsym(mysql_lib_handle, "mysql_num_fields");
							err = dlerr(errs);
						}
						if (!err) {
							num_rows = (my_ulonglong (*)(MYSQL_RES *)) dlsym(mysql_lib_handle, "mysql_num_rows");
							err = dlerr(errs);
						}
						if (!err) {
							options = (int (*)(MYSQL *, enum mysql_option, const char *)) dlsym(mysql_lib_handle,
																								"mysql_options");
							err = dlerr(errs);
						}
						if (!err) {
							real_connect = (MYSQL *(*)(MYSQL *, const char *, const char *, const char *, const char *,
													   unsigned int, const char *, unsigned long)) dlsym(
									mysql_lib_handle, "mysql_real_connect");
							err = dlerr(errs);
						}
						if (!err) {
							real_escape_string = (unsigned long (*)(MYSQL *, char *, const char *,
																	unsigned long)) dlsym(mysql_lib_handle,
																						  "mysql_real_escape_string");
							err = dlerr(errs);
						}
						if (!err) {
							real_query = (int (*)(MYSQL *, const char *, unsigned long)) dlsym(mysql_lib_handle,
																							   "mysql_real_query");
							err = dlerr(errs);
						}
						if (!err) {
							row_seek = (MYSQL_ROW_OFFSET (*)(MYSQL_RES *, MYSQL_ROW_OFFSET)) dlsym(mysql_lib_handle,
																								   "mysql_row_seek");
							err = dlerr(errs);
						}
						if (!err) {
							row_tell = (MYSQL_ROW_OFFSET (*)(MYSQL_RES *)) dlsym(mysql_lib_handle, "mysql_row_tell");
							err = dlerr(errs);
						}
						if (!err) {
							character_set_name = (const char *(*)(MYSQL *)) dlsym(mysql_lib_handle,
																				  "mysql_character_set_name");
							err = dlerr(errs);
						}
						if (!err) {
							set_character_set = (int (*)(MYSQL *, const char *)) dlsym(mysql_lib_handle,
																					   "mysql_set_character_set");
							err = dlerr(errs);
						}
						if (!err) {
							store_result = (MYSQL_RES *(*)(MYSQL *)) dlsym(mysql_lib_handle, "mysql_store_result");
							err = dlerr(errs);
						}
						if (!err) {
							mysql_stmt_init = (MYSQL_STMT* (*)(MYSQL*)) dlsym(mysql_lib_handle, "mysql_stmt_init");
							err = dlerr(errs);
						}
						if (!err) {
							mysql_stmt_close = (my_bool* (*)(MYSQL_STMT*)) dlsym(mysql_lib_handle, "mysql_stmt_close");
							err = dlerr(errs);
						}
						if (!err) {
							mysql_stmt_execute = (int (*)(MYSQL_STMT*)) dlsym(mysql_lib_handle, "mysql_stmt_execute");
							err = dlerr(errs);
						}
						if (!err) {
							mysql_stmt_prepare = (int (*)(MYSQL_STMT*, const char*, unsigned long)) dlsym(mysql_lib_handle, "mysql_stmt_prepare");
							err = dlerr(errs);
						}
						if (!err) {
							field_count = (unsigned int (*)(MYSQL *)) dlsym(mysql_lib_handle, "mysql_field_count");
							err = dlerr(errs);
						}
						if (!err) _service = true;
					}
				}
				if (_service) {
					serviceHandle = init(nullptr);
					if (serviceHandle == nullptr) {
						errs << Message(fatal, "MySQLService - initialising serviceHandle failed.");
						if (mysql_lib_handle != nullptr) {
							dlclose(mysql_lib_handle);
							mysql_lib_handle = nullptr;
						}
						if (zip_lib_handle != nullptr) {
							dlclose(zip_lib_handle);
							zip_lib_handle = nullptr;
						}
					} else {
						options(serviceHandle, MYSQL_READ_DEFAULT_GROUP, "cgi_sql_services");
						options(serviceHandle, MYSQL_SET_CHARSET_NAME, "utf8");
						errs << Message(debug, "MySQLService - initialising serviceHandle succeeded.");
					}
				}

			}
		}

		MySQLService::~MySQLService() {
			if (serviceHandle != nullptr) {
				close(serviceHandle);
			}
			if ( mysql_lib_handle != nullptr ) {
				dlclose(mysql_lib_handle);
			}
			if ( zip_lib_handle != nullptr ) {
				dlclose(zip_lib_handle);
			}
		}

		Connection* MySQLService::instance(Messages& errs) {
			return new MySQLConnection(errs,this);
		}

	}
}
