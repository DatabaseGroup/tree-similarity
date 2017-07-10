# TODO: Decide on the version.
cmake_minimum_required(VERSION 2.8)

# Create the project.
project(tree_similarity)

# Compiler flags.
# MUST be declared after project().
# TODO: Explain the flags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -O3")

# directories - for header files
#include_directories(
#  src/node
#  src/label
#  src/cost_model
#  src/zhang_shasha
#  src/data_structures
#  src/parser
#)

# Create header-only library 'TreeSimilarity'.
#
# One can also use include_directories() to add all header files which would be
# added to executables created in CMakeLists files in subdirectories.
add_library(TreeSimilarity INTERFACE)
target_include_directories(TreeSimilarity INTERFACE
  src/node
  src/label
  src/cost_model
  src/zhang_shasha
  src/data_structures
  src/parser
)

# For using add_test().
enable_testing()

# Let CMake know about subdirectories.
add_subdirectory(src/)
add_subdirectory(test/)
