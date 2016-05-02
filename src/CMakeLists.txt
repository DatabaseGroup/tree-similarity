cmake_minimum_required(VERSION 2.8)

project(tree_similarity)

# compiler flags
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -Wall -Wextra -Werror -pedantic-errors"
)

# uncomment if Boost is used
# find Boost (version >= 1.55)
#find_package(Boost 1.55 REQUIRED)

# include boost and path of CMakeLists.txt
include_directories(${Boost_INCLUDE_DIR})
include_directories(${CMAKE_BINARY_DIR})

# the set of all source files
set(tree_similarity_sources
  tree_similarity.cc
  array_2d.h
  custom_node.h
  common.h
  node.h
  string_edit_distance.h
  upper_bound.h
  zhang_shasha.h
)

# build executable
add_executable(tree_similarity ${tree_similarity_sources})

# uncomment if Boost is used
#target_link_libraries(tree_similarity ${Boost_LIBRARIES})
  
# exec error:   "./tree_similarity" "'{a}'" "'{a{b}{c}}'"
# "-"           "./tree_similarity" "'{a{b{g}}{c{f}}{del}}'" "'{a{b{g}}{c{f}}}'"
# "-"           "./tree_similarity" "'{a{b{g}}{del}{c{f}}}'" "'{a{b{g}}{c{f}}}'"
# "-"           "./tree_similarity" "'{a{del}{b{g}}{c{f}}}'" "'{a{b{g}}{c{f}}}'"
# "-"           "./tree_similarity" "'{a{b{g{del}}}{c{f}}}'" "'{a{b{g}}{c{f}}}'"
# "-"           "./tree_similarity" "'{a{b{del{g}}}{c{f}}}'" "'{a{b{g}}{c{f}}}'"
#-------------------------------------------------------------------------------
# works:        "./tree_similarity" "'{a{b{g}}{c{f}}}'" "'{a{c{f}{g}}{d}}'"
add_custom_target(execute
  "./tree_similarity" "'{a{b{g}}{c{f}}}'" "'{a{c{f}{g}}{d}}'"
)
