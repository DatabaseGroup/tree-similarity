# Zhang and Shasha algorithm tests.

# Key-root nodes testing.

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
