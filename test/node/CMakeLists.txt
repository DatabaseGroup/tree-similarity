# TreeIndexer correctness tests.

# List of tests and test files string prefixes.
set(tree_indexer_test_list
  prel_to_label_id_test
  postl_to_label_id_test
  postr_to_label_id_test
  postl_to_size_test
  prel_to_size_test
  postl_to_parent_test
  prel_to_parent_test
  postl_to_prel_test
  prel_to_postl_test
  prel_to_prer_test
  prer_to_prel_test
  prel_to_postr_test
  postr_to_prel_test
  postl_to_children_test
  prel_to_children_test
  postl_to_lld_test
  prel_to_lld_test
  prel_to_rld_test
  postr_to_rld_test
  postl_to_depth_test
  postl_to_lch_test
  postl_to_subtree_max_depth_test
  postl_to_kr_ancestor_test
  prel_to_ln_test
  prer_to_ln_test
  prel_to_type_left_test
  prel_to_type_right_test
  prel_to_spf_cost_all_test
  prel_to_spf_cost_left_test
  prel_to_spf_cost_right_test
  prel_to_subtree_del_cost_test
  prel_to_subtree_ins_cost_test
  list_kr_test
  inverted_list_depth_to_postl_test
  inverted_list_label_id_to_postl_test
)

# Test driver name.
set(
  test_driver_name
  tree_indexer_test_driver
)

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
    NAME    tree_indexer_${loop_var} # TEST NAME
    COMMAND ${test_driver_name}      # EXECUTABLE NAME
    ${loop_var}                      # ARGUMENTS
  )
endforeach(loop_var)
