# TODO: Decide on the version.
cmake_minimum_required(VERSION 2.8)

# Create the project.
project(tree_similarity)

# Compiler flags.
# MUST be declared after project().
# TODO: Explain the flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# Verify which timing implementation to use.
# NOTE: Comes from common-code.
include(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
if(NOT HAVE_CLOCK_GETTIME)
  add_definitions(-DGETRUSAGE)
endif()
# Link the timing library and header file.
add_library(
  TimingLibrary                          # LIBRARY NAME
  modules/common-code/timings/timing.cxx # LIBRARY SOURCE
)

# Create header-only library 'TreeSimilarity'.
#
# One can also use include_directories() to add all header files which would be
# added to executables created in CMakeLists files in subdirectories.
add_library(TreeSimilarity INTERFACE)
target_include_directories(TreeSimilarity INTERFACE
  src/node
  src/label
  src/cost_model
  src/ted
  src/data_structures
  src/parser
  src/join
  src/join/naive
  src/join/tjoin
  src/join/tang
  src/tree_generator
  src/ted_ub
)

# For using add_test().
enable_testing()

# Let CMake know about subdirectories.
add_subdirectory(src/)
add_subdirectory(test/)
