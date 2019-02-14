#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <clocale>
#include "test.h"
#include "support/Infix.h"
#include "support/Timing.h"
#include "support/Env.h"
#include "mt/mt.h"

int main( const int argc, const char **argv ) {
	Support::Env& env = Support::Env::e();
	env.startup(argc,argv);
	env.setTesting(true);
	testing::group tests("tests/");        // Set the working directory from the Run|Edit Configurations... menu.
	tests.load("main",false); 			  // Boolean turns on/off success reports.
	Support::Infix::Evaluate::shutdown();
	return( EXIT_SUCCESS );
}
