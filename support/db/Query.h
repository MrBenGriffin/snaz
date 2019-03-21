//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_QUERY_H
#define MACROTEXT_QUERY_H

#include <vector>
#include <map>
#include <string>

#include "Connection.h"
#include "support/Message.h"

namespace Support {
	namespace Db {
		class Query {
		protected:
			friend class Connection;
			virtual void list_tables(Messages &,TableSet&)=0;

		protected:
			std::string querystr;
			bool isactive;
			size_t numRows;
			size_t numFields;
			typedef std::map<std::string,size_t> nameIndexMap;
			nameIndexMap fieldnameidx;								//indexes of fieldnames (for name->number lookup)
			std::vector<std::string> fieldnames;			//implicitly ordered by field number (for number->name lookup)

			virtual void reset();									// reset the query		--used by vdb
			virtual void resetrows()=0;								//
			virtual bool readfield(Messages &,size_t, std::string&)=0;		// string
			virtual bool readfield(Messages &,size_t, long&)=0;				// signed
			virtual bool readfield(Messages &,size_t, long double&)=0;		// float
			virtual bool readfield(Messages &,size_t, size_t&)=0;			// size_t
			virtual bool readfield(Messages &,size_t, bool&)=0;				// bool

			virtual void readFieldName(Messages &,size_t i, std::string&, std::string&)=0;
			virtual void forQuery(Messages &,std::string& value)=0; //convert a string to make ready for query.. See Service::escape!!
			void prepareFieldCache(Messages&);
			virtual void listFields(Messages &,const std::string& table)=0;

		public:
			virtual bool nextrow()=0;								//rebuild(10+)
			virtual bool readfield(Messages &,size_t, size_t, std::string&)=0;	//module.
			virtual void readfieldforquery(Messages&,size_t,size_t,std::string&)=0;	//module. escape values so that the result is ready for a query.
			bool readfield(Messages&,const std::string&, std::string&);		//rebuild(10+)
			bool readfield(Messages&,const std::string&, size_t&);
			bool readfield(Messages&,const std::string&, long&);
			bool readfield(Messages&,const std::string&, long double&);
			bool readfield(Messages&,const std::string&, bool&);
			bool readTime(Messages&,const std::string&, ::time_t&);

			bool fieldMatch(Messages&,const std::string&, const std::string&);
			bool readfield(Messages&,size_t,const std::string&,std::string&);	//context(1),medusa(4),rebuild(3)
			size_t getnumrows() const { return numRows; }			//used env::builder (5), context (1), medusa (4), rebuild(1).
			size_t getnumfields() const {return numFields;}			//rebuild(iForSubs)
			bool fieldname(size_t,std::string&);					//rebuild(iForSubs),medusa(1)
			virtual bool execute(Messages &)=0;						// used everywhere
			void setquery(const std::string&);						// used everywhere
			explicit Query(std::string = "");
			virtual ~Query();
		};
	}
}

#endif //MACROTEXT_QUERY_H
