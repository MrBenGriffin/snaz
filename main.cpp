#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <clocale>
#include "Build.h"

//#include "support/Timing.h"
#include "support/Env.h"
#include "support/Message.h"
//#include "support/db/ServiceFactory.h"
//#include "support/db/Connection.h"

//#include "mt/mt.h"

// Currently using the following environment variables...
// LIBMYSQLCRSO=/usr/local/mysql/lib/libmysqlclient.dylib
// SQL_CONFIG_FILE=/Users/ben/Desktop/my.cnf

using namespace Support;
int main( const int argc, const char **argv ) {
	long double version=2019.031201;
	Env& env = Env::e();
	pair<Messages,Db::Connection*> services = env.startup(argc,argv);
	Messages& log = services.first;
	if (argc == 2 && argv[1][0]=='-' && argv[1][1]=='V') {
		std::cout << "Builder v" << std::setprecision(16) << version << ". Build: " << __DATE__ << "; " << __TIME__  << std::endl;
	} else {
		Build &build = Build::b(log,services.second,version);
		build.run();
		build.close(log);
	}
	return (EXIT_SUCCESS);
}

