//
// Created by Ben on 2019-02-27.
//
#include <sstream>
#include <iostream>
#include <mysql_version.h>

#include "MySQLQuery.h"
#include "support/Convert.h"

namespace Support {
	namespace Db {
		MySQLQuery::MySQLQuery(MySQLService* const s_in, MySQLConnection* const c_in, MYSQL*& h_in, std::string& qstr) :
				Query(qstr),s(s_in),c(c_in),queryHandle(h_in),result(nullptr) {
		}

		MySQLQuery::~MySQLQuery() {
			reset();
		}

		void MySQLQuery::fieldRangeCheck(Messages &errs,size_t i) {
			if (i < 1 || i > numFields) {
				ostringstream message;
				message << "MySQLQuery range error:: " << "Field index '" << i << "' out of range";
				errs << Message(range,message.str());
			}
		}

		void MySQLQuery::rowRangeCheck(Messages &errs,size_t i) {
			if (i < 1 || i > numRows) {
				ostringstream message;
				message << "MySQLQuery range error:: " << "Row index '" << i << "' out of range";
				errs << Message(range,message.str());
			}
		}

		void MySQLQuery::rowFieldRangeCheck(Messages &errs,size_t i, size_t j) {
			rowRangeCheck(errs,i);
			fieldRangeCheck(errs,j);
		}

		bool MySQLQuery::nextrow() {
			if (isactive && s->row_tell(result) != end) {
				row = s->fetch_row(result);
				mysqlRowLengths = (unsigned long*)s->fetch_lengths(result);
				return true;
			}
			return false;
		}

		void MySQLQuery::resetRows(Messages &errs) {
			if(result == nullptr) {
				errs << Message(error,"SQL: resetRows found with empty resultset.");
			} else {
				s->row_seek(result, start);
			}
		}

		void MySQLQuery::setRow(Messages &errs,size_t i) {
			if (isactive && result != nullptr) {
				try {
					rowRangeCheck(errs,i);
					s->data_seek(result, i - 1); //Seeks to an arbitrary row in a query result set. The offset value is a row number.
					row = s->fetch_row(result); //Get the next row (by cursor).
					mysqlRowLengths = (unsigned long*)s->fetch_lengths(result);
				} catch (...) {
					ostringstream message;
					message << "MySQLQuery readField error::  for row '" << i << "'";
					errs << Message(error,message.str());
				}
			} else {
				errs << Message(error,"SQL: setRow found inactive / unset");
			}
		}

		void MySQLQuery::forQuery(Messages &errs,std::string& value) {
			size_t len = value.size();
			if ( (len > 0) && (len < string::npos) ) {
				auto * buffer = new char[ len * 2 + 1];
				try {
					size_t size = s->real_escape_string(queryHandle, buffer, value.c_str(), len);
					value = std::string(buffer,size);
				}
				catch (...) { //just handle the memory allocation here.
					errs << Message(fatal,"MySQLQuery escape error:: unknown error occured with string '" + value + "'.");
					delete[] buffer;
				}
				delete[] buffer;
			}
		}

		//i is field_id, not row number..
		bool MySQLQuery::readfield(Messages &errs,size_t i, std::string& readString) {
			bool retval = false;
			if (isactive) {
				if (s->row_tell(result) != start) {
					try {
						fieldRangeCheck(errs,i);
						const char *field = row[i - 1];
						unsigned long length = mysqlRowLengths[i - 1];
						if (field != nullptr && length > 0 )  {
							readString = std::string(field, length);
						} else {
							readString = "";
						}
						retval = true;
					} catch (...) {
						ostringstream message;
						message << "MySQLQuery readField error::  for row '" << i << "'.";
						errs << Message(error,message.str());
						retval = false;
					}
				}
			}
			return retval;
		}

		const char* MySQLQuery::field(Messages &errs ,size_t i) {
			const char* value = nullptr;
			if (isactive && row != nullptr) {
				auto* rpos = s->row_tell(result);
				if (rpos != start) {
					try {
						fieldRangeCheck(errs, i);
						value = row[i - 1];
					} catch (...) {
						ostringstream message;
						message << "MySQLQuery readField error::  for row '" << i << "'.";
						errs << Message(error, message.str());
					}
				}
			}
			return value;
		}

		bool MySQLQuery::readfield(Messages &errs,size_t i, long double& value) {
			bool retval = false;
			if (isactive) {
				if (s->row_tell(result) != start) {
					try {
						fieldRangeCheck(errs,i);
						const char *field = row[i - 1];
						size_t length = mysqlRowLengths[i - 1];
						if (field != nullptr && length > 0 )  {
							retval = true;
							char* chEnd = row[i - 1] + length;
							value = strtold(field,&chEnd);
						}
					} catch (...) {
						ostringstream message;
						message << "MySQLQuery readField error::  for row '" << i << "'.";
						errs << Message(error,message.str());
						retval = false;
					}
				}
			}
			return retval;
		}

		//i is field_id, not row number..
		bool MySQLQuery::readfield(Messages &errs,size_t i, long& readLong) {
			bool retval = false;
			if (isactive) {
				if (s->row_tell(result) != start) {
					try {
						fieldRangeCheck(errs,i);
						const char *field = row[i - 1];
						unsigned long length = mysqlRowLengths[i - 1];
						if (field != nullptr && length > 0 )  {
							pair<long,bool> goodLong;
							string readString;
							readString = std::string(field, length);
							retval = true;
							goodLong = Support::integer(readString);
							if(goodLong.second) {
								readLong = goodLong.first;
							}
						}
					} catch (...) {
						ostringstream message;
						message << "MySQLQuery readField error::  for row '" << i << "'.";
						errs << Message(error,message.str());
						retval = false;
					}
				}
			}
			return retval;
		}

		bool MySQLQuery::readfield(Messages &errs,size_t i, unsigned long long& value) {
			bool retval = false;
			if (isactive) {
				if (s->row_tell(result) != start) {
					try {
						fieldRangeCheck(errs,i);
						const char *field = row[i - 1];
						size_t length = mysqlRowLengths[i - 1];
						if (field != nullptr && length > 0 )  {
							string readString;
							readString = std::string(field, length);
							auto in=readString.begin();
							value = 0;
							while(isdigit(*in) ) {
								value = value * 10 + *in - '0';
								in++;
							}
							retval = true;
						}
					} catch (...) {
						ostringstream message;
						message << "MySQLQuery readField error::  for row '" << i << "'.";
						errs << Message(error,message.str());
						retval = false;
					}
				}
			}
			return retval;
		}
		//i is field_id, not row number..
		bool MySQLQuery::readfield(Messages &errs,size_t i, unsigned long& readUnsigned) {
			bool retval = false;
			if (isactive) {
				if (s->row_tell(result) != start) {
					try {
						fieldRangeCheck(errs,i);
						const char *field = row[i - 1];
						size_t length = mysqlRowLengths[i - 1];
						if (field != nullptr && length > 0 )  {
							string readString;
							readString = std::string(field, length);
							readUnsigned = Support::natural(readString);
							retval = true;
						}
					} catch (...) {
						ostringstream message;
						message << "MySQLQuery readField error::  for row '" << i << "'.";
						errs << Message(error,message.str());
						retval = false;
					}
				}
			}
			return retval;
		}

		bool MySQLQuery::readfield(Messages &errs,size_t i, bool& readBool) {
			bool retval = false;
			if (isactive) {
				if (s->row_tell(result) != start) {
					try {
						fieldRangeCheck(errs,i);
						const char *field = row[i - 1];
						size_t length = mysqlRowLengths[i - 1];
						if (field != nullptr && length > 0 )  {
							string readString;
							readString = std::string(field, length);
							readBool = (readString == "1") || (readString == "on");
							retval = true;
						}
					} catch (...) {
						ostringstream message;
						message << "MySQLQuery readField error::  for row '" << i << "'.";
						errs << Message(error,message.str());
						retval = false;
					}
				}
			}
			return retval;
		}

		bool MySQLQuery::readfield(Messages &errs,size_t i, size_t j, std::string& readString) {
			bool retval = false;
			if (isactive) {
				try {
					rowFieldRangeCheck(errs, i, j);
					MYSQL_ROW_OFFSET currowoff = s->row_tell(result);
					s->data_seek(result, i - 1);
					MYSQL_ROW currow = s->fetch_row(result);
					const char* field = currow[j - 1];
					size_t length = s->fetch_lengths(result)[j - 1];
					s->row_seek(result, currowoff);
					if (length > 0) {
						readString.assign(field, length);
					} else {
						readString.assign("");
					}
					retval = true;
				} catch (...) {
					ostringstream message;
					message << "MySQLQuery readField error::  for row '" << i << "', field '" << j << "'.";
					errs << Message(error,message.str());
					retval = false;
				}
			}
			return retval;
		}

		void MySQLQuery::readfieldforquery(Messages &errs,size_t i, size_t j, std::string& readString) {
			readfield(errs,i,j,readString);
			forQuery(errs,readString);
		}

		//This is a private function used by vdb only.
		void MySQLQuery::list_tables(Messages &errs,TableSet& ts) {
			if (execute(errs)) {
				while (nextrow()) {
					const char* tname = row[0];
					if(tname) {
						ts.insert(string(tname));
					}
				}
			}
		}

		void MySQLQuery::listFields(Messages &errs,const std::string& table) {
			if (isactive) {
				reset();
			}
			result = s->list_fields(queryHandle, table.c_str(), nullptr);
			prepareResult(errs);
		}

		void MySQLQuery::reset() {
			if (result != nullptr) {
				s->free_result(result);
				result = nullptr;
			}
			row = nullptr;
			start = nullptr;
			end = nullptr;
			Query::reset();
		}

		bool MySQLQuery::check(Messages &errs) {
			bool value = s->my_errno(queryHandle) == 0;
			if(!value) {
//				auto ss = queryHandle->server_status;
//				auto cs = queryHandle->status;

				ostringstream str;
				string serverError(s->error(queryHandle));
				str << "SQL:" << serverError;
				errs << Message(error,str.str());
				errs << Message(info,querystr);
				reset();
			}
			return value;
		}

		bool MySQLQuery::execute(Messages &errs) {
			bool retval = false;
			ostringstream str;
			try {
				if (isactive) { reset(); }
				int err = s->real_query(queryHandle, querystr.c_str(), querystr.length());
				if (err) {
					string serverError(s->error(queryHandle));
					str << "SQL error:" << serverError;
					errs << Message(error,str.str()); str.str("");
					str << "Offending query:" << querystr;
					errs << Message(info,str.str());
					row = nullptr;
					start = nullptr;
					result = nullptr;
					reset();
				} else {
					numFields = s->field_count(queryHandle);
					if(numFields > 0) {
						if(check(errs)) {
							result = s->store_result(queryHandle);
							if(check(errs)) {
								prepareResult(errs);
								retval = true;
							}
						}
					} else {
						if(check(errs)) {
							retval = true;
						}
					}
				}
			}
			catch (...) {
				str << "MySQLQuery execute error for query '" << querystr << "'.";
				errs << Message(error,str.str());
			}
			return retval;
		}

		void MySQLQuery::prepareResult(Messages &errs) {
			if (result != nullptr) {
				numRows = (int) s->num_rows(result); //64 bit to 32 bit conversion...
				numFields = s->num_fields(result);
				if (numRows > 0) {
					start = s->row_tell(result);
					s->data_seek(result, numRows);
					end = s->row_tell(result);
					s->row_seek(result, start);
					isactive = true;
					prepareFieldCache(errs);
				} else {
					reset();
				}
			} else {
				row = nullptr;
				start = nullptr;
				result = nullptr;
				Query::reset();
				isactive = false;
			}
		}

		void MySQLQuery::readFieldName(Messages &errs,size_t i, std::string& fname, std::string& tname) {
			if (isactive) {
				fieldRangeCheck(errs,i);
				MYSQL_FIELD* field = s->fetch_field_direct(result, (unsigned int)(i - 1));
				if ( field != nullptr && field-> name != nullptr) {
					fname.assign(field->name);
					if (field->table != nullptr) tname.assign(field->table);
				}
			} else {
				errs << Message(fatal,"MySQLQuery readFieldName:: Result set is not active to read field '" + fname + "'.");
			}
		}

	}
}
