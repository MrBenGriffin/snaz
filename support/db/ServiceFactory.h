//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_SERVICEFACTORY_H
#define MACROTEXT_SERVICEFACTORY_H

#include <string>
#include <map>

#include "support/Message.h"

namespace Support {
	namespace Db {
		class Service;
		class Connection;
		class ServiceFactory {
		private:
			std::map<std::string, Service*> serviceMap;

			 ServiceFactory() = default;
			~ServiceFactory();

		public:
			static ServiceFactory& sf();
			Service* getService(Messages&,const std::string&);
			Connection* getConnection(Messages&,const std::string&,const std::string&);
//			if(Env::e().get("SQL_CONFIG_FILE",configFile)) {
//				retval = open(errs, configFilePath );
//			} else {
//				errs << Message(fatal,"SQL config file at SQL_CONFIG_FILE not found");
//			}

		};
	}
}



#endif //MACROTEXT_SERVICEFACTORY_H
