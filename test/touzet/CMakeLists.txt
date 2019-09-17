# Touzet's algorithm tests.

# Specify list of test strings.
set(touzet_test_list
  touzet_subtree_size_test
  touzet_node_depth_test
  touzet_subtree_max_depth_test
  touzet_depth_inverted_list_test
  touzet_td_test
  touzet_subproblem_count_depth_pruning_test
  touzet_ted_test
  touzet_kr_test
  touzet_left_child_test
  touzet_parent_test
  touzet_nodes_kr_test
)

# Add custom target for building all touzet's tests.
add_custom_target(touzet_all_tests)

# Specify test strings in the loop.
foreach(loop_var ${touzet_test_list})
  file(
    COPY        ${loop_var}_data.txt
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
  )
  add_executable(
    ${loop_var}_driver # EXECUTABLE NAME
    ${loop_var}.cc     # EXECUTABLE SOURCE
  )
  target_link_libraries(
    ${loop_var}_driver # EXECUTABLE NAME
    TreeSimilarity     # LIBRARY NAME
  )
  target_link_libraries(
    ${loop_var}_driver # EXECUTABLE NAME
    TestCommon         # LIBRARY NAME
  )
  add_test(
    NAME    ${loop_var}        # TEST NAME
    COMMAND ${loop_var}_driver # EXECUTABLE NAME
  )
  # Add this target to touzet_all_tests target.
  add_dependencies(
    touzet_all_tests   # TARGET
    ${loop_var}_driver # TARGET DEPENDENCY
  )
endforeach(loop_var)

