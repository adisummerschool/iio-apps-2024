# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/analog/workspace/practica/iio-apps-2024/day1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/analog/workspace/practica/iio-apps-2024/day1/build

# Include any dependencies generated for this target.
include CMakeFiles/myproj.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/myproj.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/myproj.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/myproj.dir/flags.make

CMakeFiles/myproj.dir/main.c.o: CMakeFiles/myproj.dir/flags.make
CMakeFiles/myproj.dir/main.c.o: /home/analog/workspace/practica/iio-apps-2024/day1/main.c
CMakeFiles/myproj.dir/main.c.o: CMakeFiles/myproj.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/analog/workspace/practica/iio-apps-2024/day1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/myproj.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/myproj.dir/main.c.o -MF CMakeFiles/myproj.dir/main.c.o.d -o CMakeFiles/myproj.dir/main.c.o -c /home/analog/workspace/practica/iio-apps-2024/day1/main.c

CMakeFiles/myproj.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/myproj.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/analog/workspace/practica/iio-apps-2024/day1/main.c > CMakeFiles/myproj.dir/main.c.i

CMakeFiles/myproj.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/myproj.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/analog/workspace/practica/iio-apps-2024/day1/main.c -o CMakeFiles/myproj.dir/main.c.s

# Object files for target myproj
myproj_OBJECTS = \
"CMakeFiles/myproj.dir/main.c.o"

# External object files for target myproj
myproj_EXTERNAL_OBJECTS =

myproj: CMakeFiles/myproj.dir/main.c.o
myproj: CMakeFiles/myproj.dir/build.make
myproj: CMakeFiles/myproj.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/analog/workspace/practica/iio-apps-2024/day1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable myproj"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/myproj.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/myproj.dir/build: myproj
.PHONY : CMakeFiles/myproj.dir/build

CMakeFiles/myproj.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/myproj.dir/cmake_clean.cmake
.PHONY : CMakeFiles/myproj.dir/clean

CMakeFiles/myproj.dir/depend:
	cd /home/analog/workspace/practica/iio-apps-2024/day1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/analog/workspace/practica/iio-apps-2024/day1 /home/analog/workspace/practica/iio-apps-2024/day1 /home/analog/workspace/practica/iio-apps-2024/day1/build /home/analog/workspace/practica/iio-apps-2024/day1/build /home/analog/workspace/practica/iio-apps-2024/day1/build/CMakeFiles/myproj.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/myproj.dir/depend

