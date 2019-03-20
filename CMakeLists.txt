cmake_minimum_required(VERSION 3.13)
project(macrotext)

#cf. https://github.com/Genivia/RE-flex for a new version of flex..

find_package(BISON 3.2)
find_package(FLEX 2.6)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};/usr/local/include;/usr/local/mysql/include;/usr/include/mysql)
#find_library(MYSQLCLIENT libmysqlclient HINTS "/usr/local/mysql/lib" )
#MESSAGE("MYSQL LIB: " ${MYSQLCLIENT})

link_libraries(${CMAKE_DL_LIBS})

FLEX_TARGET(advanced mt/advanced.lpp ${CMAKE_CURRENT_BINARY_DIR}/advanced.yy.cpp COMPILE_FLAGS "-d")
FLEX_TARGET(classic mt/classic.lpp ${CMAKE_CURRENT_BINARY_DIR}/classic.yy.cpp COMPILE_FLAGS "-d ")
BISON_TARGET(parser mt/parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp COMPILE_FLAGS "-Wother -Wconflicts-sr")
ADD_FLEX_BISON_DEPENDENCY(advanced parser)
ADD_FLEX_BISON_DEPENDENCY(classic parser)


add_executable(macrotext ${BISON_parser_OUTPUTS} ${FLEX_advanced_OUTPUTS} ${FLEX_classic_OUTPUTS}
        mt/Advanced.h mt/Classic.h mt/Scanner.h mt/declarations.h
        mt/Content.cpp mt/Macro.cpp mt/Text.cpp mt/Wss.cpp mt/Definition.cpp mt/Injection.cpp
        mt/Driver.cpp
        mt/Instance.cpp mt/Instance.h
        mt/Internal.cpp
        mt/InternalInstance.cpp
        mt/Internals.h
        mt/InternalsBuild.cpp
        mt/InternalsEncoder.cpp
        mt/InternalsMedia.cpp
        mt/InternalsNode.cpp
        mt/InternalsNodeTree.cpp
        mt/InternalsStorage.cpp
        mt/InternalsString.cpp
        mt/InternalsTaxonomy.cpp
        mt/InternalsUtility.cpp
        support/Definitions.h support/Definitions.cpp
        support/db/Service.h
        support/db/Connection.cpp support/db/Connection.h
        support/db/MySQLConnection.cpp support/db/MySQLConnection.h
        support/db/MySQLQuery.cpp support/db/MySQLQuery.h
        support/db/MySQLService.cpp support/db/MySQLService.h
        support/db/Query.cpp support/db/Query.h
        support/db/ServiceFactory.cpp support/db/ServiceFactory.h
        support/Dynamic.h
        support/Comparison.cpp support/Comparison.h
        support/Convert.cpp support/Convert.h
        support/Date.cpp support/Date.h
        support/Encode.cpp support/Encode.h
        support/Env.cpp support/Env.h
        support/Fandr.cpp support/Fandr.h
        support/File.cpp support/File.h
        support/Infix.cpp support/Infix.h
        support/Message.cpp support/Message.h
        support/Regex.cpp support/Regex.h
        support/Storage.cpp support/Storage.h
        support/Timing.cpp support/Timing.h
        node/Tree.cpp node/Tree.h
        node/Node.cpp node/Node.h
        node/NodeFilename.cpp node/NodeFilename.h
        node/BadLocatorPath.h
        node/Locator.cpp node/Locator.h
        node/Content.cpp node/Content.h
        node/Suffix.cpp node/Suffix.h
        node/Taxon.cpp node/Taxon.h
        main.cpp test.cpp Build.cpp
        )
