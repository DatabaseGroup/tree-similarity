cmake_minimum_required(VERSION 2.8)

project(tree_similarity)
set(PROJECT_NAME_EXEC "./${PROJECT_NAME}")

# compiler flags
set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -g"
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
  data_structures/array_2d.h
  nodes/custom_node.h
  common/common.h
  nodes/node.h
  sed/string_edit_distance.h
  upper_bound/upper_bound.h
  zhang_shasha/zhang_shasha.h
  nodes/string_node.h
  data_structures/k_heap.h
  wrappers/node_distance_pair.h
  wrappers/node_prefixes_pair.h
  tasm/ring_buffer.h
  tasm/tasm.h
)

# build executable
add_executable(${PROJECT_NAME} ${tree_similarity_sources})

# uncomment if Boost is used
#target_link_libraries(tree_similarity ${Boost_LIBRARIES})
 
# OLD: 
# exec error:   "./tree_similarity" "'{a}'" "'{a{b}{c}}'"
# "-"           "./tree_similarity" "'{a{b{g}}{c{f}}{del}}'" "'{a{b{g}}{c{f}}}'"
# "-"           "./tree_similarity" "'{a{b{g}}{del}{c{f}}}'" "'{a{b{g}}{c{f}}}'"
# "-"           "./tree_similarity" "'{a{del}{b{g}}{c{f}}}'" "'{a{b{g}}{c{f}}}'"
# "-"           "./tree_similarity" "'{a{b{g{del}}}{c{f}}}'" "'{a{b{g}}{c{f}}}'"
# "-"           "./tree_similarity" "'{a{b{del{g}}}{c{f}}}'" "'{a{b{g}}{c{f}}}'"
#-------------------------------------------------------------------------------
# works:        "./tree_similarity" "'{a{b{g}}{c{f}}}'" "'{a{c{f}{g}}{d}}'"

# set test inputs (from stdin)
set(TEST_TREE_STDIN_001 "'{a{b{c}{d{e}{f}}}{x}}}'")
set(TEST_TREE_STDIN_002 "'{b{c}{d{e}{f}}}'")

add_custom_target(execute
  "${PROJECT_NAME_EXEC}" "${TEST_TREE_STDIN_001}" "${TEST_TREE_STDIN_002}"
)

# dynamic code analysis - valgrind
find_program(CHECK_DYNAMIC_COMMAND valgrind)
add_custom_target(check-dynamic "${CHECK_DYNAMIC_COMMAND}"
  "-v"
  "--leak-check=full"
  "--show-reachable=yes"
  "--track-origins=yes"
  "--trace-children=yes"
  "${PROJECT_NAME_EXEC}"
  "${TEST_TREE_STDIN_001}"
  "${TEST_TREE_STDIN_002}"
)

# static code analysis - cppcheck
find_program(CHECK_STATIC_COMMAND cppcheck)
add_custom_target(check-static "${CHECK_STATIC_COMMAND}"
  "--enable=all"
  "--suppress=missingIncludeSystem"
  "${PROJECT_SOURCE_DIR}"
)

# static and dynamic code analysis
add_custom_target(check DEPENDS check-static check-dynamic)
