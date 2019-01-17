#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <clocale>
#include "test.h"

int main( const int argc, const char **argv ) {
   std::setlocale(LC_ALL, "en_UK.UTF-8");
   testing::group tests("/Users/ben/CLionProjects/snazzle/tests/");
   tests.load("main",false); 							// boolean turns on/off success reports.
   return( EXIT_SUCCESS );
}
