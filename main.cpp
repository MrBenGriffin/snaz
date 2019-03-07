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
// =/Users/ben/Desktop/my.cnf

using namespace Support;
int main( const int argc, const char **argv ) {
	Env& env = Env::e();
	Build& build = Build::b();
	env.startup(argc,argv);
	build.setCurrent(Testing); //For this..
	Messages log = std::move(env.startup(argc,argv));
	testing::group tests("tests/");       // Set the working directory from the Run|Edit Configurations... menu.
	tests.load("main",false); 			  // Boolean turns on/off success reports.
	Infix::Evaluate::shutdown();
	log.str(std::cout);
	return( EXIT_SUCCESS );
}

