cmake_minimum_required(VERSION 2.8)

# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall -O3")

# directories
include_directories(
  ${CMAKE_BINARY_DIR}
  node
  label
  cost_model
  zhang_shasha
  data_structures
  parser
)

# the set of all source files
set(tree_similarity_sources
  tree_similarity.h
  tree_similarity.cc
  node/node.h
  label/string_label.h
  label/string_label.cc
  cost_model/unit_cost_model.h
  zhang_shasha/zhang_shasha.h
  data_structures/matrix.h
  parser/bracket_notation_parser.h
)

# build executable
add_executable(${PROJECT_NAME} ${tree_similarity_sources})
