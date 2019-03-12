//
// Created by Ben on 2019-02-27.
//
#include <sstream>
#include <utility>

#include "support/Env.h"
#include "Query.h"

namespace Support {
	namespace Db {

		Query::Query(std::string q) : querystr(std::move(std::move(q))),isactive(false),numRows(-1),numFields(-1),fieldnameidx(),fieldnames() {	}

		Query::~Query(){
			fieldnameidx.clear();
			fieldnames.clear();
		}

		void Query::reset() {
			querystr.clear();
			numRows = -1;
			numFields = -1;
			fieldnameidx.clear();
			fieldnames.clear();
			isactive = false;
		}

		void Query::setquery(const std::string& newquerystr) {
			numRows = -1;
			numFields = -1;
			isactive = false;
			fieldnameidx.clear();
			fieldnames.clear();
			querystr = newquerystr;
		}

		bool Query::fieldname(size_t idx,std::string& result) {
			if (isactive && idx <= fieldnames.size() ) {
				result = fieldnames[idx-1];
				return true;
			} else {
				return false;
			}
		}

		bool Query::fieldMatch(Messages& errs,const std::string& field, const std::string& test) {
			if (isactive) {
				string value;
				auto it = fieldnameidx.find(field);
				if (it != fieldnameidx.end()) {
					readfield(errs, it->second,value);
					return value == test;
				} else {
					errs << Message(fatal,"field " + field + "does not exist for fieldMatch.");
				}
			} else {
				errs << Message(fatal,"Query/Connection is not active.");
			}
			return false;
		}

		bool Query::readfield(Messages& errs,const std::string& field, std::string& readString) {
			if (isactive) {
				auto it = fieldnameidx.find(field);
				if (it != fieldnameidx.end()) {
					readfield(errs, it->second, readString);
					return true;
				}
			}
			return false;
		}

		bool Query::readfield(Messages& errs,const std::string& field, long double& readDouble) {
			if (isactive) {
				auto it = fieldnameidx.find(field);
				if (it != fieldnameidx.end()) {
					readfield(errs, it->second, readDouble);
					return true;
				}
			}
			return false;
		}

		bool Query::readfield(Messages& errs,const std::string& field, size_t& readUnsized) {
			if (isactive) {
				auto it = fieldnameidx.find(field);
				if (it != fieldnameidx.end()) {
					readfield(errs, it->second, readUnsized);
					return true;
				}
			}
			return false;
		}


		bool Query::readfield(Messages& errs,const std::string& field, long& readLong) {
			if (isactive) {
				auto it = fieldnameidx.find(field);
				if (it != fieldnameidx.end()) {
					readfield(errs, it->second, readLong);
					return true;
				}
			}
			return false;
		}

		bool Query::readfield(Messages& errs,size_t i,const std::string& field, std::string& readString ) {
			if (isactive) {
				auto it = fieldnameidx.find(field);
				if (it != fieldnameidx.end()) {
					readfield(errs, i, it->second, readString);
					return true;
				}
			}
			return false;
		}

		void Query::prepareFieldCache(Messages& errs) {
			if (isactive) {
				for(size_t i = 1; i <= numFields; i++) {
					std::string fname,tname;
					std::ostringstream tfname;
					readFieldName(errs, i, fname, tname);
					tfname <<  tname << '.' << fname;
					nameIndexMap::const_iterator ni = fieldnameidx.find(fname);
					if ( ni != fieldnameidx.end() ) {
						errs << Message(fatal,"SQL Error: Field " + fname + " is ambiguous in the query " + querystr);
					} else {
						fieldnameidx.insert(nameIndexMap::value_type(fname, i));
						fieldnameidx.insert(nameIndexMap::value_type(tfname.str(), i));
						fieldnames.push_back(fname);
					}
				}
			} else {
				errs << Message(fatal,"SQL Error: fieldCaches cannot be prepared for inactive queries.");
			}
		}

	}
}
