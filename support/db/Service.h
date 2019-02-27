//
// Created by Ben on 2019-02-27.
//

#ifndef MACROTEXT_SERVICE_H
#define MACROTEXT_SERVICE_H
#include <string>
#include "support/Message.h"

namespace Support {
	namespace Db {
		class Connection;

		class Service {
		protected:
			bool _service;	//used to show if the service is up or down.
			Service() : _service(false) {}

		public:
			bool service() { return _service; };
			// Connection instance.
			// This doesn't connect (use open for that), just sets up ready to connect
			// (eg creates a handle)...
			virtual Connection* instance(Messages& ) =0;
			virtual ~Service() = default;

		};
	}
}

#endif //MACROTEXT_SERVICE_H
