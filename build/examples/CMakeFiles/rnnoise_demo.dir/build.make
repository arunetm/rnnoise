# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/arun/rnnoise

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/arun/rnnoise/build

# Include any dependencies generated for this target.
include examples/CMakeFiles/rnnoise_demo.dir/depend.make

# Include the progress variables for this target.
include examples/CMakeFiles/rnnoise_demo.dir/progress.make

# Include the compile flags for this target's objects.
include examples/CMakeFiles/rnnoise_demo.dir/flags.make

examples/CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.o: examples/CMakeFiles/rnnoise_demo.dir/flags.make
examples/CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.o: ../examples/rnnoise_demo.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/arun/rnnoise/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object examples/CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.o"
	cd /home/arun/rnnoise/build/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.o   -c /home/arun/rnnoise/examples/rnnoise_demo.c

examples/CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.i"
	cd /home/arun/rnnoise/build/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/arun/rnnoise/examples/rnnoise_demo.c > CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.i

examples/CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.s"
	cd /home/arun/rnnoise/build/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/arun/rnnoise/examples/rnnoise_demo.c -o CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.s

# Object files for target rnnoise_demo
rnnoise_demo_OBJECTS = \
"CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.o"

# External object files for target rnnoise_demo
rnnoise_demo_EXTERNAL_OBJECTS =

bin/rnnoise_demo: examples/CMakeFiles/rnnoise_demo.dir/rnnoise_demo.c.o
bin/rnnoise_demo: examples/CMakeFiles/rnnoise_demo.dir/build.make
bin/rnnoise_demo: examples/CMakeFiles/rnnoise_demo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/arun/rnnoise/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable ../bin/rnnoise_demo"
	cd /home/arun/rnnoise/build/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rnnoise_demo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
examples/CMakeFiles/rnnoise_demo.dir/build: bin/rnnoise_demo

.PHONY : examples/CMakeFiles/rnnoise_demo.dir/build

examples/CMakeFiles/rnnoise_demo.dir/clean:
	cd /home/arun/rnnoise/build/examples && $(CMAKE_COMMAND) -P CMakeFiles/rnnoise_demo.dir/cmake_clean.cmake
.PHONY : examples/CMakeFiles/rnnoise_demo.dir/clean

examples/CMakeFiles/rnnoise_demo.dir/depend:
	cd /home/arun/rnnoise/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/arun/rnnoise /home/arun/rnnoise/examples /home/arun/rnnoise/build /home/arun/rnnoise/build/examples /home/arun/rnnoise/build/examples/CMakeFiles/rnnoise_demo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/CMakeFiles/rnnoise_demo.dir/depend

