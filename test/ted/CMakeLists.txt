# TED algorithms correctness tests.

# List of tests and test string prefixes.
set(ted_test_list
  zhang_shasha
  apted
  touzet_baseline
  touzet_depth_pruning
  touzet_depth_pruning_truncated_tree_fix
  touzet_kr_loop
  touzet_kr_set
)

# Test driver name.
set(
  test_driver_name
  ted_correctness_test_driver
)

add_executable(
  ${test_driver_name}  # EXECUTABLE NAME
  ted_test.cc # EXECUTABLE SOURCE
)
target_link_libraries(
  ${test_driver_name} # EXECUTABLE NAME
  TreeSimilarity      # LIBRARY NAME
)
target_link_libraries(
  ${test_driver_name} # EXECUTABLE NAME
  TestCommon          # LIBRARY NAME
)

# Copy test cases.
file(
  COPY        ted_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# Create tests in a loop. Use the same executable.
foreach(loop_var ${ted_test_list})
  add_test(
    NAME    ted_correctness_test_${loop_var} # TEST NAME
    COMMAND ${test_driver_name}  # EXECUTABLE NAME
    ${loop_var}                  # ARGUMENTS
  )
endforeach(loop_var)