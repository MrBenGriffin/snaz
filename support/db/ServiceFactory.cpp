//
// Created by Ben on 2019-02-27.
//

#include "ServiceFactory.h"
#include "Service.h"
#include "Connection.h"
#include "MySQLService.h"

namespace Support {
	namespace Db {

		ServiceFactory& ServiceFactory::sf() {
			static ServiceFactory singleton;
			return singleton;
		}

		Service* ServiceFactory::getService(Messages& errs,const std::string& service_name) {
			Service* retval = nullptr;
			auto it = serviceMap.find(service_name);
			if (it != serviceMap.end()) {
				retval = it->second;
			} else {
//Add db service types here...
				if (service_name == "mysql") {
					retval = new MySQLService(errs);
					if ( retval->service()  ) {
						serviceMap.insert({"mysql",retval});
					} else {
						delete retval; //service not available.
						retval = nullptr;
					}

				}
			}
			return retval;
		}

		Connection* ServiceFactory::getConnection(Messages& errs,const std::string& service_name) {
			auto* service = getService(errs,service_name);
			if(service) {
				auto* conn = service->instance(errs);
				conn->connect(errs);
				return conn;
			}
			return nullptr;
		}

		ServiceFactory::~ServiceFactory() {
			//automatically called at exit()
			for(auto& i : serviceMap) {
				delete i.second;
			}
			serviceMap.clear();
		}

	}
}

