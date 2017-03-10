cmake_minimum_required(VERSION 2.8)

# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -O3")

# directories
include_directories(
  ${CMAKE_BINARY_DIR}
  node
)

# the set of all source files
set(tree_similarity_sources
  tree_similarity.h
  tree_similarity.cc
  node/node.h
)

# build executable
add_executable(${PROJECT_NAME} ${tree_similarity_sources})
