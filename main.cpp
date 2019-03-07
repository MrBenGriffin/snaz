#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <clocale>
#include "test.h"
#include "Build.h"

#include "support/Infix.h"
#include "support/Timing.h"
#include "support/Env.h"
#include "support/Message.h"
#include "support/db/ServiceFactory.h"
#include "support/db/Connection.h"

#include "mt/mt.h"

// Currently using the following environment variables...
// LIBMYSQLCRSO=/usr/local/mysql/lib/libmysqlclient.dylib
// SQL_CONFIG_FILE=/Users/ben/Desktop/my.cnf

using namespace Support;
int main( const int argc, const char **argv ) {
	Env& env = Env::e();
	pair<Messages&,Db::Connection*> services = env.startup(argc,argv);

	Messages& log = services.first;
	Db::Connection* sql = services.second;

	Build& build = Build::b(log,sql);
	build.setCurrent(Testing); //For this..
	log.str(std::cout);
	testing::group tests("tests/");   		// Set the working directory from the Run|Edit Configurations... menu.
	tests.title(std::cout,"Main");
	tests.load(std::cout, "main", false);   // Boolean turns on/off success reports.
	log.str(std::cout);

	Infix::Evaluate::shutdown();
	return( EXIT_SUCCESS );

}

