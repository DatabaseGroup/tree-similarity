# TODO: Decide on the version.
cmake_minimum_required(VERSION 3.7)

# Create the project.
project(tree_similarity)

# Copied from https://stackoverflow.com/questions/14933172/how-can-i-add-a-minimum-compiler-version-requisite/14934542#14934542
# Use "export CXX=clang++-3.9" to point cmake to use clang.
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # require at least gcc 7.0
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        message(FATAL_ERROR "GCC version must be at least 7.0!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # require at least clang 3.9
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.9)
        message(FATAL_ERROR "Clang version must be at least 3.9!")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    # require at least MSVC 17.0
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17.0)
        message(FATAL_ERROR "MSVC version must be at least 17.0! (Lower versions haven't been tested)")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! Compilation has only been tested with Clang and GCC.")
endif()

# TODO: Make it compile under Windows without Visual Studio.
#       Document the steps.

# Compiler flags.
# MUST be declared after project().
# TODO: Explain the flags.
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wextra -Wpedantic")
#Define CXX standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Add flags applicable to all compilers
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#Add compiler specific flags
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wpedantic")
endif(CMAKE_COMPILER_IS_GNUCXX)

# It's possible to set compiler-specific flags.
#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall")
#endif()

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
  src/lookup
  src/lookup/scan
  src/lookup/index
  src/join
  src/join/naive
  src/join/tjoin
  src/join/tang
  src/join/degree_histogram
  src/join/leaf_dist_histogram
  src/join/label_histogram
  src/join/binary_branches
  src/join/histogram
  src/join/guha
  src/tree_generator
  src/ted_ub
  src/ted_lb
  src/json
)

# For using add_test().
include(CTest)

# Let CMake know about subdirectories.
add_subdirectory(src/)
add_subdirectory(test/)



# Define install target (used by VCPKG)
# install and export target
install(
  TARGETS TreeSimilarity
  EXPORT TreeSimilarity-targets)

#install(
#  DIRECTORY .
#  DESTINATION ${TreeSimilarity_INSTALL_INCLUDE_DIR}/tree-similiarity)

#install(
#  EXPORT TreeSimilarity-targets
#  FILE tree-similiarity-config.cmake
#  NAMESPACE tree-similiarity::
#  DESTINATION ${tree_similiarity_INSTALL_CMAKE_DIR}/tree-similiarity)
  
#install(
#  FILES ${CMAKE_CURRENT_BINARY_DIR}/tree-similiarity-config-version.cmake
#  DESTINATION ${tree_similiarity_INSTALL_CMAKE_DIR}/tree-similiarity)

