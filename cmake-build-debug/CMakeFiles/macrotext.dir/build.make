# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = "/Users/ben/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/183.4886.39/CLion.app/Contents/bin/cmake/mac/bin/cmake"

# The command to remove a file.
RM = "/Users/ben/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/183.4886.39/CLion.app/Contents/bin/cmake/mac/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ben/CLionProjects/snaz

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ben/CLionProjects/snaz/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/macrotext.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/macrotext.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/macrotext.dir/flags.make

parser.tab.cpp: ../parser.ypp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "[BISON][parser] Building parser with bison 3.2.4"
	cd /Users/ben/CLionProjects/snaz && /usr/local/bin/bison -d -o /Users/ben/CLionProjects/snaz/cmake-build-debug/parser.tab.cpp parser.ypp

parser.tab.hpp: parser.tab.cpp
	@$(CMAKE_COMMAND) -E touch_nocreate parser.tab.hpp

advanced.yy.cpp: ../advanced.lpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "[FLEX][advanced] Building scanner with flex 2.6.4"
	cd /Users/ben/CLionProjects/snaz && /usr/local/bin/flex -d -o/Users/ben/CLionProjects/snaz/cmake-build-debug/advanced.yy.cpp advanced.lpp

CMakeFiles/macrotext.dir/parser.tab.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/parser.tab.cpp.o: parser.tab.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/macrotext.dir/parser.tab.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/parser.tab.cpp.o -c /Users/ben/CLionProjects/snaz/cmake-build-debug/parser.tab.cpp

CMakeFiles/macrotext.dir/parser.tab.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/parser.tab.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/cmake-build-debug/parser.tab.cpp > CMakeFiles/macrotext.dir/parser.tab.cpp.i

CMakeFiles/macrotext.dir/parser.tab.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/parser.tab.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/cmake-build-debug/parser.tab.cpp -o CMakeFiles/macrotext.dir/parser.tab.cpp.s

CMakeFiles/macrotext.dir/advanced.yy.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/advanced.yy.cpp.o: advanced.yy.cpp
CMakeFiles/macrotext.dir/advanced.yy.cpp.o: parser.tab.hpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/macrotext.dir/advanced.yy.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/advanced.yy.cpp.o -c /Users/ben/CLionProjects/snaz/cmake-build-debug/advanced.yy.cpp

CMakeFiles/macrotext.dir/advanced.yy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/advanced.yy.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/cmake-build-debug/advanced.yy.cpp > CMakeFiles/macrotext.dir/advanced.yy.cpp.i

CMakeFiles/macrotext.dir/advanced.yy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/advanced.yy.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/cmake-build-debug/advanced.yy.cpp -o CMakeFiles/macrotext.dir/advanced.yy.cpp.s

CMakeFiles/macrotext.dir/main.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/macrotext.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/main.cpp.o -c /Users/ben/CLionProjects/snaz/main.cpp

CMakeFiles/macrotext.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/main.cpp > CMakeFiles/macrotext.dir/main.cpp.i

CMakeFiles/macrotext.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/main.cpp -o CMakeFiles/macrotext.dir/main.cpp.s

CMakeFiles/macrotext.dir/Driver.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/Driver.cpp.o: ../Driver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/macrotext.dir/Driver.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/Driver.cpp.o -c /Users/ben/CLionProjects/snaz/Driver.cpp

CMakeFiles/macrotext.dir/Driver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/Driver.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/Driver.cpp > CMakeFiles/macrotext.dir/Driver.cpp.i

CMakeFiles/macrotext.dir/Driver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/Driver.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/Driver.cpp -o CMakeFiles/macrotext.dir/Driver.cpp.s

CMakeFiles/macrotext.dir/test.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/test.cpp.o: ../test.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/macrotext.dir/test.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/test.cpp.o -c /Users/ben/CLionProjects/snaz/test.cpp

CMakeFiles/macrotext.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/test.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/test.cpp > CMakeFiles/macrotext.dir/test.cpp.i

CMakeFiles/macrotext.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/test.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/test.cpp -o CMakeFiles/macrotext.dir/test.cpp.s

CMakeFiles/macrotext.dir/storage.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/storage.cpp.o: ../storage.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/macrotext.dir/storage.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/storage.cpp.o -c /Users/ben/CLionProjects/snaz/storage.cpp

CMakeFiles/macrotext.dir/storage.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/storage.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/storage.cpp > CMakeFiles/macrotext.dir/storage.cpp.i

CMakeFiles/macrotext.dir/storage.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/storage.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/storage.cpp -o CMakeFiles/macrotext.dir/storage.cpp.s

CMakeFiles/macrotext.dir/userMacro.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/userMacro.cpp.o: ../userMacro.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/macrotext.dir/userMacro.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/userMacro.cpp.o -c /Users/ben/CLionProjects/snaz/userMacro.cpp

CMakeFiles/macrotext.dir/userMacro.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/userMacro.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/userMacro.cpp > CMakeFiles/macrotext.dir/userMacro.cpp.i

CMakeFiles/macrotext.dir/userMacro.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/userMacro.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/userMacro.cpp -o CMakeFiles/macrotext.dir/userMacro.cpp.s

CMakeFiles/macrotext.dir/Injection.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/Injection.cpp.o: ../Injection.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/macrotext.dir/Injection.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/Injection.cpp.o -c /Users/ben/CLionProjects/snaz/Injection.cpp

CMakeFiles/macrotext.dir/Injection.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/Injection.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/Injection.cpp > CMakeFiles/macrotext.dir/Injection.cpp.i

CMakeFiles/macrotext.dir/Injection.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/Injection.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/Injection.cpp -o CMakeFiles/macrotext.dir/Injection.cpp.s

CMakeFiles/macrotext.dir/Wss.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/Wss.cpp.o: ../Wss.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/macrotext.dir/Wss.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/Wss.cpp.o -c /Users/ben/CLionProjects/snaz/Wss.cpp

CMakeFiles/macrotext.dir/Wss.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/Wss.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/Wss.cpp > CMakeFiles/macrotext.dir/Wss.cpp.i

CMakeFiles/macrotext.dir/Wss.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/Wss.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/Wss.cpp -o CMakeFiles/macrotext.dir/Wss.cpp.s

CMakeFiles/macrotext.dir/Macro.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/Macro.cpp.o: ../Macro.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/macrotext.dir/Macro.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/Macro.cpp.o -c /Users/ben/CLionProjects/snaz/Macro.cpp

CMakeFiles/macrotext.dir/Macro.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/Macro.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/Macro.cpp > CMakeFiles/macrotext.dir/Macro.cpp.i

CMakeFiles/macrotext.dir/Macro.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/Macro.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/Macro.cpp -o CMakeFiles/macrotext.dir/Macro.cpp.s

CMakeFiles/macrotext.dir/Text.cpp.o: CMakeFiles/macrotext.dir/flags.make
CMakeFiles/macrotext.dir/Text.cpp.o: ../Text.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/macrotext.dir/Text.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/macrotext.dir/Text.cpp.o -c /Users/ben/CLionProjects/snaz/Text.cpp

CMakeFiles/macrotext.dir/Text.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/macrotext.dir/Text.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ben/CLionProjects/snaz/Text.cpp > CMakeFiles/macrotext.dir/Text.cpp.i

CMakeFiles/macrotext.dir/Text.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/macrotext.dir/Text.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ben/CLionProjects/snaz/Text.cpp -o CMakeFiles/macrotext.dir/Text.cpp.s

# Object files for target macrotext
macrotext_OBJECTS = \
"CMakeFiles/macrotext.dir/parser.tab.cpp.o" \
"CMakeFiles/macrotext.dir/advanced.yy.cpp.o" \
"CMakeFiles/macrotext.dir/main.cpp.o" \
"CMakeFiles/macrotext.dir/Driver.cpp.o" \
"CMakeFiles/macrotext.dir/test.cpp.o" \
"CMakeFiles/macrotext.dir/storage.cpp.o" \
"CMakeFiles/macrotext.dir/userMacro.cpp.o" \
"CMakeFiles/macrotext.dir/Injection.cpp.o" \
"CMakeFiles/macrotext.dir/Wss.cpp.o" \
"CMakeFiles/macrotext.dir/Macro.cpp.o" \
"CMakeFiles/macrotext.dir/Text.cpp.o"

# External object files for target macrotext
macrotext_EXTERNAL_OBJECTS =

macrotext: CMakeFiles/macrotext.dir/parser.tab.cpp.o
macrotext: CMakeFiles/macrotext.dir/advanced.yy.cpp.o
macrotext: CMakeFiles/macrotext.dir/main.cpp.o
macrotext: CMakeFiles/macrotext.dir/Driver.cpp.o
macrotext: CMakeFiles/macrotext.dir/test.cpp.o
macrotext: CMakeFiles/macrotext.dir/storage.cpp.o
macrotext: CMakeFiles/macrotext.dir/userMacro.cpp.o
macrotext: CMakeFiles/macrotext.dir/Injection.cpp.o
macrotext: CMakeFiles/macrotext.dir/Wss.cpp.o
macrotext: CMakeFiles/macrotext.dir/Macro.cpp.o
macrotext: CMakeFiles/macrotext.dir/Text.cpp.o
macrotext: CMakeFiles/macrotext.dir/build.make
macrotext: CMakeFiles/macrotext.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Linking CXX executable macrotext"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/macrotext.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/macrotext.dir/build: macrotext

.PHONY : CMakeFiles/macrotext.dir/build

CMakeFiles/macrotext.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/macrotext.dir/cmake_clean.cmake
.PHONY : CMakeFiles/macrotext.dir/clean

CMakeFiles/macrotext.dir/depend: parser.tab.cpp
CMakeFiles/macrotext.dir/depend: parser.tab.hpp
CMakeFiles/macrotext.dir/depend: advanced.yy.cpp
	cd /Users/ben/CLionProjects/snaz/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ben/CLionProjects/snaz /Users/ben/CLionProjects/snaz /Users/ben/CLionProjects/snaz/cmake-build-debug /Users/ben/CLionProjects/snaz/cmake-build-debug /Users/ben/CLionProjects/snaz/cmake-build-debug/CMakeFiles/macrotext.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/macrotext.dir/depend

