#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <clocale>
#include "test.h"
#include "support/Infix.h"
#include "support/Timing.h"
#include "support/Env.h"
#include "support/db/ServiceFactory.h"
#include "support/db/Connection.h"

#include "mt/mt.h"

int main( const int argc, const char **argv ) {
	Support::Env& env = Support::Env::e();
	env.startup(argc,argv);
	env.setTesting(true);
	Support::Messages log;
	auto* mysql = Support::Db::ServiceFactory::sf().getConnection(log,"mysql");
//	mysql->exec(log,"insert into bldvar(name,value) values ('foo','bar')");
	testing::group tests("tests/");       // Set the working directory from the Run|Edit Configurations... menu.
	tests.load("main",false); 			  // Boolean turns on/off success reports.
	Support::Infix::Evaluate::shutdown();
	log.str(std::cout);
	return( EXIT_SUCCESS );
}
