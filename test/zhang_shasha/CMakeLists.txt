# Zhang and Shasha algorithm tests.

# Key-root nodes testing.

# Copy test cases.
file(
  COPY key_roots_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  node_indexing_key_roots_test_driver # EXECUTABLE NAME
  node_indexing_key_roots_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  node_indexing_key_roots_test_driver # EXECUTABLE NAME
  TreeSimilarity                      # LIBRARY NAME
)

add_test(
  NAME node_indexing_key_roots_test           # TEST NAME
  COMMAND node_indexing_key_roots_test_driver # EXECUTABLE NAME
)

# Leftmost leaf descendants testing.

# Copy test cases.
file(
  COPY lld_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  node_indexing_lld_test_driver # EXECUTABLE NAME
  node_indexing_lld_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  node_indexing_lld_test_driver # EXECUTABLE NAME
  TreeSimilarity                # LIBRARY NAME
)

add_test(
  NAME node_indexing_lld_test           # TEST NAME
  COMMAND node_indexing_lld_test_driver # EXECUTABLE NAME
)

# TED testing.

# Copy test cases.
file(
  COPY ted_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  ted_test_driver # EXECUTABLE NAME
  ted_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  ted_test_driver # EXECUTABLE NAME
  TreeSimilarity  # LIBRARY NAME
)

add_test(
  NAME ted_test           # TEST NAME
  COMMAND ted_test_driver # EXECUTABLE NAME
)
