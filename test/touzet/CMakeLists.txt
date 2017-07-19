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
