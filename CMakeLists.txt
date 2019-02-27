cmake_minimum_required(VERSION 3.13)
project(macrotext)

#cf. https://github.com/Genivia/RE-flex for a new version of flex..

find_package(BISON 3.2)
find_package(FLEX 2.6)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};/usr/local/include;/usr/local/mysql/include)
#find_library(MYSQLCLIENT libmysqlclient HINTS "/usr/local/mysql/lib" )
#MESSAGE("MYSQL LIB: " ${MYSQLCLIENT})

FLEX_TARGET(advanced mt/advanced.lpp ${CMAKE_CURRENT_BINARY_DIR}/advanced.yy.cpp COMPILE_FLAGS "-d")
FLEX_TARGET(classic mt/classic.lpp ${CMAKE_CURRENT_BINARY_DIR}/classic.yy.cpp COMPILE_FLAGS "-d ")
BISON_TARGET(parser mt/parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp)
ADD_FLEX_BISON_DEPENDENCY(advanced parser)
ADD_FLEX_BISON_DEPENDENCY(classic parser)


add_executable(macrotext ${BISON_parser_OUTPUTS} ${FLEX_advanced_OUTPUTS} ${FLEX_classic_OUTPUTS} main.cpp
        mt/Driver.cpp test.cpp support/Storage.cpp mt/Definition.cpp mt/Injection.cpp mt/Wss.cpp mt/Macro.cpp mt/Text.cpp
        mt/Internal.cpp mt/InternalInstance.cpp mt/Content.cpp support/Infix.cpp support/Message.cpp support/Encode.cpp
        support/Fandr.cpp support/Convert.cpp mt/declarations.h mt/Scanner.h mt/Advanced.h mt/Classic.h
        mt/InternalsStorage.cpp mt/InternalsUtility.cpp mt/InternalsString.cpp support/Comparison.cpp support/Comparison.h
        support/Regex.cpp support/Regex.h support/Dynamic.h mt/Internals.h mt/Instance.cpp mt/Instance.h
        support/Timing.cpp support/Timing.h support/File.cpp support/File.h support/Date.cpp
        support/Date.h support/Env.cpp support/Env.h mt/InternalsEncoder.cpp mt/InternalsNodeTree.cpp
        mt/InternalsBuild.cpp mt/InternalsNode.cpp mt/InternalsMedia.cpp mt/InternalsTaxonomy.cpp
        support/db/ServiceFactory.cpp support/db/ServiceFactory.h support/db/Service.h
        support/db/MySQLService.cpp support/db/MySQLService.h support/db/Connection.cpp support/db/Connection.h support/db/Query.cpp support/db/Query.h support/db/MySQLConnection.cpp support/db/MySQLConnection.h support/db/MySQLQuery.cpp support/db/MySQLQuery.h)
