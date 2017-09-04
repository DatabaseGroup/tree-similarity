# Touzet's algorithm tests.

# Subtree sizes testing.

# Copy test cases.
file(
  COPY subtree_size_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  touzet_node_indexing_subtree_size_test_driver # EXECUTABLE NAME
  touzet_node_indexing_subtree_size_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  touzet_node_indexing_subtree_size_test_driver # EXECUTABLE NAME
  TreeSimilarity                                # LIBRARY NAME
)

add_test(
  NAME touzet_node_indexing_subtree_size_test           # TEST NAME
  COMMAND touzet_node_indexing_subtree_size_test_driver # EXECUTABLE NAME
)

# Node depth testing.

# Copy test cases.
file(
  COPY touzet_node_depth_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  touzet_node_indexing_node_depth_test_driver # EXECUTABLE NAME
  touzet_node_indexing_node_depth_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  touzet_node_indexing_node_depth_test_driver # EXECUTABLE NAME
  TreeSimilarity                              # LIBRARY NAME
)

add_test(
  NAME touzet_node_indexing_node_depth_test           # TEST NAME
  COMMAND touzet_node_indexing_node_depth_test_driver # EXECUTABLE NAME
)

# Depth inverted list testing.

# Copy test cases.
file(
  COPY touzet_depth_inverted_list_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  touzet_node_indexing_depth_inverted_list_test_driver # EXECUTABLE NAME
  touzet_node_indexing_depth_inverted_list_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  touzet_node_indexing_depth_inverted_list_test_driver # EXECUTABLE NAME
  TreeSimilarity                                       # LIBRARY NAME
)

add_test(
  NAME touzet_node_indexing_depth_inverted_list_test           # TEST NAME
  COMMAND touzet_node_indexing_depth_inverted_list_test_driver # EXECUTABLE NAME
)

# td-matrix testing.

# Copy test cases.
file(
  COPY td_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  touzet_td_test_driver # EXECUTABLE NAME
  touzet_td_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  touzet_td_test_driver # EXECUTABLE NAME
  TreeSimilarity        # LIBRARY NAME
)

add_test(
  NAME touzet_td_test           # TEST NAME
  COMMAND touzet_td_test_driver # EXECUTABLE NAME
)

# Subproblem count for depth-based prunning testing.

# Copy test cases.
file(
  COPY touzet_subproblem_count_depth_pruning_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  touzet_subproblem_count_depth_pruning_test_driver # EXECUTABLE NAME
  touzet_subproblem_count_depth_pruning_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  touzet_subproblem_count_depth_pruning_test_driver # EXECUTABLE NAME
  TreeSimilarity                                    # LIBRARY NAME
)

add_test(
  NAME touzet_subproblem_count_depth_pruning_test           # TEST NAME
  COMMAND touzet_subproblem_count_depth_pruning_test_driver # EXECUTABLE NAME
)

# TED testing.

# Copy test cases.
file(
  COPY touzet_ted_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  touzet_ted_test_driver # EXECUTABLE NAME
  touzet_ted_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  touzet_ted_test_driver # EXECUTABLE NAME
  TreeSimilarity         # LIBRARY NAME
)

add_test(
  NAME touzet_ted_test           # TEST NAME
  COMMAND touzet_ted_test_driver # EXECUTABLE NAME
)
