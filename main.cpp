#include <iostream>
#include <iomanip>
#include "Build.h"

#include "support/Env.h"
#include "support/Message.h"
#include "support/Timing.h"

// Currently using the following environment variables...
// LIBMYSQLCRSO=/usr/local/mysql/lib/libmysqlclient.dylib
// SQL_CONFIG_FILE=/Users/ben/Desktop/my.cnf

using namespace Support;
int main( const int argc, const char **argv ) {
	Env& env = Env::e();
	pair< unique_ptr<Messages>,Db::Connection*> services = env.startup(argc,argv);
	auto log = std::move(services.first);
	std::ostringstream title;
	title << "Builder v" << std::setprecision(16) << Build::version << ". Build: " << __DATE__ << "; " << __TIME__ ;
	if (argc == 2 && argv[1][0]=='-' && argv[1][1]=='V') {
		std::cout << title.str()  << std::endl;
	} else {
		Timing::t(); //initialise.
		Build &build = Build::b();
		log->push(Message(info,title.str()));
		build.run(*log,services.second);
		log->pop();
		build.close(*log);
	}
	return (EXIT_SUCCESS);
}

