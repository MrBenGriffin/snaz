#include <iostream>
#include <iomanip>
#include "Build.h"

#include "support/Env.h"
#include "support/Message.h"

// Currently using the following environment variables...
// LIBMYSQLCRSO=/usr/local/mysql/lib/libmysqlclient.dylib
// SQL_CONFIG_FILE=/Users/ben/Desktop/my.cnf

using namespace Support;
int main( const int argc, const char **argv ) {
	Env& env = Env::e();
	pair<Messages,Db::Connection*> services = env.startup(argc,argv);
	Messages& log = services.first;
	if (argc == 2 && argv[1][0]=='-' && argv[1][1]=='V') {
		std::cout << "Builder v" << std::setprecision(16) << Build::version << ". Build: " << __DATE__ << "; " << __TIME__  << std::endl;
	} else {
		Build &build = Build::b(log);
		build.run(log,services.second);
		build.close(log);
	}
	return (EXIT_SUCCESS);
}

