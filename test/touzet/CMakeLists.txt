# Touzet's algorithm tests.

# Specify list of test strings.
set(touzet_test_list
  touzet_td_test
  touzet_subproblem_count_depth_pruning_test
  touzet_ted_test
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

