# TreeIndexer correctness tests.

# List of tests and test files string prefixes.
set(tree_indexer_test_list
  postl_to_label_id_test
  postl_to_size_test
  postl_to_lld_test
)

# Test driver name.
set(
  test_driver_name
  tree_indexer_test_driver
)

# All tests name.
set(
  all_tests_name
  tree_indexer_all_tests
)

# Add custom target for building all tree indexer tests.
add_custom_target(${all_tests_name})

add_executable(
  ${test_driver_name}  # EXECUTABLE NAME
  tree_indexer_test.cc # EXECUTABLE SOURCE
)
target_link_libraries(
  ${test_driver_name} # EXECUTABLE NAME
  TreeSimilarity      # LIBRARY NAME
)
target_link_libraries(
  ${test_driver_name} # EXECUTABLE NAME
  TestCommon          # LIBRARY NAME
)

# Create tests in a loop. Use the same executable.
foreach(loop_var ${tree_indexer_test_list})
  # Copy test cases.
  file(
    COPY        ${loop_var}_data.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
  )
  add_test(
    NAME    ${loop_var}         # TEST NAME
    COMMAND ${test_driver_name} # EXECUTABLE NAME
    ${loop_var}                 # ARGUMENTS
  )
  # Add this target to tree_indexer_all_tests target.
  add_dependencies(
    ${all_tests_name}   # TARGET
    ${test_driver_name} # TARGET DEPENDENCY
  )
endforeach(loop_var)
