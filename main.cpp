#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <clocale>
#include "test.h"
#include "mt.h"

int main( const int argc, const char **argv ) {
   std::setlocale(LC_ALL, "en_UK.UTF-8");
   mt::Definition::initialise();

   testing::group tests("tests/");        // Set the working directory from the Run|Edit Configurations... menu.
   tests.load("main",false); 							// boolean turns on/off success reports.
//   mt::Definition::list(std::cout);
   return( EXIT_SUCCESS );
}
