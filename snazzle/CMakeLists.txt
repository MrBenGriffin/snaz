cmake_minimum_required(VERSION 3.13)
project(macrotext)

#cf. https://github.com/Genivia/RE-flex for a new version of flex..

find_package(BISON 3.2)
find_package(FLEX 2.6)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_BINARY_DIR};/usr/local/opt/flex/include)

FLEX_TARGET(advanced advanced.lpp ${CMAKE_CURRENT_BINARY_DIR}/advanced.yy.cpp COMPILE_FLAGS -d)
BISON_TARGET(parser parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp)
ADD_FLEX_BISON_DEPENDENCY(advanced parser)

add_executable(macrotext ${BISON_parser_OUTPUTS} ${FLEX_advanced_OUTPUTS} main.cpp Driver.cpp test.cpp test.h storage.cpp storage.h userMacro.cpp userMacro.h)
