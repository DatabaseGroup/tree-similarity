# Touzet's algorithm tests.

# Specify list of test strings.
set(greedy_ub_test_list
  greedy_ub_label_il_test
  greedy_ub_mapping_test
)

# Add custom target for building all touzet's tests.
add_custom_target(greedy_ub_all_tests)

# Specify test strings in the loop.
foreach(loop_var ${greedy_ub_test_list})
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
  add_test(
    NAME    ${loop_var}        # TEST NAME
    COMMAND ${loop_var}_driver # EXECUTABLE NAME
  )
  # Add this target to greedy_ub_all_tests target.
  add_dependencies(
    greedy_ub_all_tests   # TARGET
    ${loop_var}_driver # TARGET DEPENDENCY
  )
endforeach(loop_var)

# Link the timing library with greedy_ub_cost_test_driver.
#target_link_libraries(
#  greedy_ub_cost_test_driver # TARGET EXECUTABLE NAME
#  TimingLibrary          # LIBRARY NAME
#)
#target_include_directories(
#  greedy_ub_cost_test_driver                    # TARGET EXECUTABLE NAME
#  PUBLIC ${PROJECT_SOURCE_DIR}/modules/common-code/timings # HEADER FILE
#)

