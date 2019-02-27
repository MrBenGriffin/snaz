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
		class ServiceFactory {
		private:
			std::map<std::string, Service*> serviceMap;

			 ServiceFactory() = default;
			~ServiceFactory();

		public:
			static ServiceFactory& sf();
			Service* getService(Messages&,const std::string&);
		};
	}
}



#endif //MACROTEXT_SERVICEFACTORY_H
