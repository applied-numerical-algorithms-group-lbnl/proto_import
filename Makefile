# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /home/edavis/Apps/clion-2019.1.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/edavis/Apps/clion-2019.1.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/edavis/Work/proto

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/edavis/Work/proto

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/home/edavis/Apps/clion-2019.1.4/bin/cmake/linux/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/home/edavis/Apps/clion-2019.1.4/bin/cmake/linux/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target test
test:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running tests..."
	/home/edavis/Apps/clion-2019.1.4/bin/cmake/linux/bin/ctest --force-new-ctest-process $(ARGS)
.PHONY : test

# Special rule for the target test
test/fast: test

.PHONY : test/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/edavis/Work/proto/CMakeFiles /home/edavis/Work/proto/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/edavis/Work/proto/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named eulerTest

# Build rule for target.
eulerTest: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 eulerTest
.PHONY : eulerTest

# fast build rule for target.
eulerTest/fast:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/build
.PHONY : eulerTest/fast

examples/Euler/src/EulerOp.o: examples/Euler/src/EulerOp.cpp.o

.PHONY : examples/Euler/src/EulerOp.o

# target to build an object file
examples/Euler/src/EulerOp.cpp.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/examples/Euler/src/EulerOp.cpp.o
.PHONY : examples/Euler/src/EulerOp.cpp.o

examples/Euler/src/EulerOp.i: examples/Euler/src/EulerOp.cpp.i

.PHONY : examples/Euler/src/EulerOp.i

# target to preprocess a source file
examples/Euler/src/EulerOp.cpp.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/examples/Euler/src/EulerOp.cpp.i
.PHONY : examples/Euler/src/EulerOp.cpp.i

examples/Euler/src/EulerOp.s: examples/Euler/src/EulerOp.cpp.s

.PHONY : examples/Euler/src/EulerOp.s

# target to generate assembly for a file
examples/Euler/src/EulerOp.cpp.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/examples/Euler/src/EulerOp.cpp.s
.PHONY : examples/Euler/src/EulerOp.cpp.s

pdfl/lib/gtest/src/gtest-death-test.o: pdfl/lib/gtest/src/gtest-death-test.cc.o

.PHONY : pdfl/lib/gtest/src/gtest-death-test.o

# target to build an object file
pdfl/lib/gtest/src/gtest-death-test.cc.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-death-test.cc.o
.PHONY : pdfl/lib/gtest/src/gtest-death-test.cc.o

pdfl/lib/gtest/src/gtest-death-test.i: pdfl/lib/gtest/src/gtest-death-test.cc.i

.PHONY : pdfl/lib/gtest/src/gtest-death-test.i

# target to preprocess a source file
pdfl/lib/gtest/src/gtest-death-test.cc.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-death-test.cc.i
.PHONY : pdfl/lib/gtest/src/gtest-death-test.cc.i

pdfl/lib/gtest/src/gtest-death-test.s: pdfl/lib/gtest/src/gtest-death-test.cc.s

.PHONY : pdfl/lib/gtest/src/gtest-death-test.s

# target to generate assembly for a file
pdfl/lib/gtest/src/gtest-death-test.cc.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-death-test.cc.s
.PHONY : pdfl/lib/gtest/src/gtest-death-test.cc.s

pdfl/lib/gtest/src/gtest-filepath.o: pdfl/lib/gtest/src/gtest-filepath.cc.o

.PHONY : pdfl/lib/gtest/src/gtest-filepath.o

# target to build an object file
pdfl/lib/gtest/src/gtest-filepath.cc.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-filepath.cc.o
.PHONY : pdfl/lib/gtest/src/gtest-filepath.cc.o

pdfl/lib/gtest/src/gtest-filepath.i: pdfl/lib/gtest/src/gtest-filepath.cc.i

.PHONY : pdfl/lib/gtest/src/gtest-filepath.i

# target to preprocess a source file
pdfl/lib/gtest/src/gtest-filepath.cc.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-filepath.cc.i
.PHONY : pdfl/lib/gtest/src/gtest-filepath.cc.i

pdfl/lib/gtest/src/gtest-filepath.s: pdfl/lib/gtest/src/gtest-filepath.cc.s

.PHONY : pdfl/lib/gtest/src/gtest-filepath.s

# target to generate assembly for a file
pdfl/lib/gtest/src/gtest-filepath.cc.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-filepath.cc.s
.PHONY : pdfl/lib/gtest/src/gtest-filepath.cc.s

pdfl/lib/gtest/src/gtest-port.o: pdfl/lib/gtest/src/gtest-port.cc.o

.PHONY : pdfl/lib/gtest/src/gtest-port.o

# target to build an object file
pdfl/lib/gtest/src/gtest-port.cc.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-port.cc.o
.PHONY : pdfl/lib/gtest/src/gtest-port.cc.o

pdfl/lib/gtest/src/gtest-port.i: pdfl/lib/gtest/src/gtest-port.cc.i

.PHONY : pdfl/lib/gtest/src/gtest-port.i

# target to preprocess a source file
pdfl/lib/gtest/src/gtest-port.cc.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-port.cc.i
.PHONY : pdfl/lib/gtest/src/gtest-port.cc.i

pdfl/lib/gtest/src/gtest-port.s: pdfl/lib/gtest/src/gtest-port.cc.s

.PHONY : pdfl/lib/gtest/src/gtest-port.s

# target to generate assembly for a file
pdfl/lib/gtest/src/gtest-port.cc.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-port.cc.s
.PHONY : pdfl/lib/gtest/src/gtest-port.cc.s

pdfl/lib/gtest/src/gtest-printers.o: pdfl/lib/gtest/src/gtest-printers.cc.o

.PHONY : pdfl/lib/gtest/src/gtest-printers.o

# target to build an object file
pdfl/lib/gtest/src/gtest-printers.cc.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-printers.cc.o
.PHONY : pdfl/lib/gtest/src/gtest-printers.cc.o

pdfl/lib/gtest/src/gtest-printers.i: pdfl/lib/gtest/src/gtest-printers.cc.i

.PHONY : pdfl/lib/gtest/src/gtest-printers.i

# target to preprocess a source file
pdfl/lib/gtest/src/gtest-printers.cc.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-printers.cc.i
.PHONY : pdfl/lib/gtest/src/gtest-printers.cc.i

pdfl/lib/gtest/src/gtest-printers.s: pdfl/lib/gtest/src/gtest-printers.cc.s

.PHONY : pdfl/lib/gtest/src/gtest-printers.s

# target to generate assembly for a file
pdfl/lib/gtest/src/gtest-printers.cc.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-printers.cc.s
.PHONY : pdfl/lib/gtest/src/gtest-printers.cc.s

pdfl/lib/gtest/src/gtest-test-part.o: pdfl/lib/gtest/src/gtest-test-part.cc.o

.PHONY : pdfl/lib/gtest/src/gtest-test-part.o

# target to build an object file
pdfl/lib/gtest/src/gtest-test-part.cc.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-test-part.cc.o
.PHONY : pdfl/lib/gtest/src/gtest-test-part.cc.o

pdfl/lib/gtest/src/gtest-test-part.i: pdfl/lib/gtest/src/gtest-test-part.cc.i

.PHONY : pdfl/lib/gtest/src/gtest-test-part.i

# target to preprocess a source file
pdfl/lib/gtest/src/gtest-test-part.cc.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-test-part.cc.i
.PHONY : pdfl/lib/gtest/src/gtest-test-part.cc.i

pdfl/lib/gtest/src/gtest-test-part.s: pdfl/lib/gtest/src/gtest-test-part.cc.s

.PHONY : pdfl/lib/gtest/src/gtest-test-part.s

# target to generate assembly for a file
pdfl/lib/gtest/src/gtest-test-part.cc.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest-test-part.cc.s
.PHONY : pdfl/lib/gtest/src/gtest-test-part.cc.s

pdfl/lib/gtest/src/gtest.o: pdfl/lib/gtest/src/gtest.cc.o

.PHONY : pdfl/lib/gtest/src/gtest.o

# target to build an object file
pdfl/lib/gtest/src/gtest.cc.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest.cc.o
.PHONY : pdfl/lib/gtest/src/gtest.cc.o

pdfl/lib/gtest/src/gtest.i: pdfl/lib/gtest/src/gtest.cc.i

.PHONY : pdfl/lib/gtest/src/gtest.i

# target to preprocess a source file
pdfl/lib/gtest/src/gtest.cc.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest.cc.i
.PHONY : pdfl/lib/gtest/src/gtest.cc.i

pdfl/lib/gtest/src/gtest.s: pdfl/lib/gtest/src/gtest.cc.s

.PHONY : pdfl/lib/gtest/src/gtest.s

# target to generate assembly for a file
pdfl/lib/gtest/src/gtest.cc.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/pdfl/lib/gtest/src/gtest.cc.s
.PHONY : pdfl/lib/gtest/src/gtest.cc.s

test/EulerTest.o: test/EulerTest.cpp.o

.PHONY : test/EulerTest.o

# target to build an object file
test/EulerTest.cpp.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/test/EulerTest.cpp.o
.PHONY : test/EulerTest.cpp.o

test/EulerTest.i: test/EulerTest.cpp.i

.PHONY : test/EulerTest.i

# target to preprocess a source file
test/EulerTest.cpp.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/test/EulerTest.cpp.i
.PHONY : test/EulerTest.cpp.i

test/EulerTest.s: test/EulerTest.cpp.s

.PHONY : test/EulerTest.s

# target to generate assembly for a file
test/EulerTest.cpp.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/test/EulerTest.cpp.s
.PHONY : test/EulerTest.cpp.s

test/driver.o: test/driver.cpp.o

.PHONY : test/driver.o

# target to build an object file
test/driver.cpp.o:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/test/driver.cpp.o
.PHONY : test/driver.cpp.o

test/driver.i: test/driver.cpp.i

.PHONY : test/driver.i

# target to preprocess a source file
test/driver.cpp.i:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/test/driver.cpp.i
.PHONY : test/driver.cpp.i

test/driver.s: test/driver.cpp.s

.PHONY : test/driver.s

# target to generate assembly for a file
test/driver.cpp.s:
	$(MAKE) -f CMakeFiles/eulerTest.dir/build.make CMakeFiles/eulerTest.dir/test/driver.cpp.s
.PHONY : test/driver.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... eulerTest"
	@echo "... test"
	@echo "... examples/Euler/src/EulerOp.o"
	@echo "... examples/Euler/src/EulerOp.i"
	@echo "... examples/Euler/src/EulerOp.s"
	@echo "... pdfl/lib/gtest/src/gtest-death-test.o"
	@echo "... pdfl/lib/gtest/src/gtest-death-test.i"
	@echo "... pdfl/lib/gtest/src/gtest-death-test.s"
	@echo "... pdfl/lib/gtest/src/gtest-filepath.o"
	@echo "... pdfl/lib/gtest/src/gtest-filepath.i"
	@echo "... pdfl/lib/gtest/src/gtest-filepath.s"
	@echo "... pdfl/lib/gtest/src/gtest-port.o"
	@echo "... pdfl/lib/gtest/src/gtest-port.i"
	@echo "... pdfl/lib/gtest/src/gtest-port.s"
	@echo "... pdfl/lib/gtest/src/gtest-printers.o"
	@echo "... pdfl/lib/gtest/src/gtest-printers.i"
	@echo "... pdfl/lib/gtest/src/gtest-printers.s"
	@echo "... pdfl/lib/gtest/src/gtest-test-part.o"
	@echo "... pdfl/lib/gtest/src/gtest-test-part.i"
	@echo "... pdfl/lib/gtest/src/gtest-test-part.s"
	@echo "... pdfl/lib/gtest/src/gtest.o"
	@echo "... pdfl/lib/gtest/src/gtest.i"
	@echo "... pdfl/lib/gtest/src/gtest.s"
	@echo "... test/EulerTest.o"
	@echo "... test/EulerTest.i"
	@echo "... test/EulerTest.s"
	@echo "... test/driver.o"
	@echo "... test/driver.i"
	@echo "... test/driver.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

