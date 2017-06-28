cmake_minimum_required(VERSION 2.8)

# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -O3")

# directories - for header files
include_directories(
  node
  label
  cost_model
  zhang_shasha
  data_structures
  parser
)

# the set of all source files (only the cc files)
set(tree_similarity_sources
  tree_similarity.cc
)

# build executable
add_executable(${PROJECT_NAME} ${tree_similarity_sources})
