# Create test executable 'node_indexing_test_driver' based on 'node_indexing_test.cc'.
add_executable(node_indexing_test_driver zhang_shasha/node_indexing_test.cc)

# Let the compiler know to find the header files in TreeSimilarity library.
# Link 'TreeSimilarity' library to 'node_indexing_test_driver' executable.
# Such that the header files of the library can be used in the test file.
target_link_libraries(node_indexing_test_driver TreeSimilarity)

# Create the test 'node_indexing_test' based on executable 'node_indexing_test_driver'.
# Command-line arguments, if used, can be passed here.
# The test will be executed with 'make test' after 'make'.
add_test(node_indexing_test node_indexing_test_driver)
