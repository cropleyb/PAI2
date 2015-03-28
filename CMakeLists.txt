cmake_minimum_required(VERSION 2.8)

project(my_project)

# General
###############################################################################
# Activate C++0x support (if you encounter gmock errors, use 1.6.0 version)
add_definitions(-std=c++0x)

# Extra warnings
add_definitions(-Wextra)

# Google Mock & Test
###############################################################################
add_subdirectory(${GMOCK_ROOT})
include_directories(${GMOCK_ROOT}/include)
include_directories(${GMOCK_ROOT}/gtest/include)

# Main program executable
###############################################################################
set(EXECUTABLE_NAME "program")

# Valgrind
###############################################################################
function(add_memcheck_test name binary)
  set(memcheck_command "${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS}")
  separate_arguments(memcheck_command)
  add_test(${name} ${binary} ${ARGN})
  add_test(memcheck_${name} ${memcheck_command} ./${binary} ${ARGN})
endfunction(add_memcheck_test)

function(set_memcheck_test_properties name)
  set_tests_properties(${name} ${ARGN})
  set_tests_properties(memcheck_${name} ${ARGN})
endfunction(set_memcheck_test_properties)

# For callgrind
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -g -O3")
set(CMAKE_BUILD_TYPE Debug)
# For max speed
# set(CMAKE_CXX_FLAGS_DEBUG "-O3")

# cmake -G ... (to configure your build)
# ctest -D ExperimentalBuild (this will build your code)
# ctest -R testName -D ExperimentalTest (just runs the test)
# ctest -R testName -D ExperimentalMemCheck (to run the test under valgrind)

# Source files
file(GLOB SRC_FILES "src/*.cpp")
file(GLOB MAIN_SRC "src/Main.cpp")
file(GLOB RUNAI_SRC "src/run_ai.cpp")
list(REMOVE_ITEM SRC_FILES ${MAIN_SRC})
list(REMOVE_ITEM SRC_FILES ${RUNAI_SRC})

# Executable dependencies
if(SRC_FILES)
	add_executable("${EXECUTABLE_NAME}" ${SRC_FILES} ${MAIN_SRC}) 
endif(SRC_FILES)

# Set link libraries
# target_link_libraries($EXECUTABLE_NAME ...)

###############################################################################
# RunAI executable
###############################################################################
set(RUNAI_EXECUTABLE_NAME "runai")

if(SRC_FILES)
	add_executable("${RUNAI_EXECUTABLE_NAME}" ${SRC_FILES} ${RUNAI_SRC}) 
endif(SRC_FILES)

###############################################################################
# Tests executable
###############################################################################
set(TEST_EXECUTABLE_NAME "tests")

# Test files
file(GLOB TEST_FILES "test/*.cpp")

add_executable("${TEST_EXECUTABLE_NAME}" ${TEST_FILES} ${SRC_FILES})
add_memcheck_test("${MEMCHECK_TEST_EXECUTABLE_NAME}" ${TEST_FILES} ${SRC_FILES})

# Set link libraries (order matters)
target_link_libraries("${TEST_EXECUTABLE_NAME}" gmock gtest pthread)

# Include root directory
include_directories(BEFORE ".")
include_directories(BEFORE "src/")

###############################################################################
# Library
###############################################################################

add_library("pai2" SHARED ${SRC_FILES})
