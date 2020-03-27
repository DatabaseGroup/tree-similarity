# LGM Upper Bound algorithm tests.

# Specify list of test strings.
set(lgm_ub_test_list
  lgm_ub_lb_mapping_test
  lgm_ub_lb_fill_gaps_test
)

# Add custom target for building all LGM tests.
add_custom_target(lgm_ub_all_tests)

# Specify test strings in the loop.
foreach(loop_var ${lgm_ub_test_list})
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
  # Add this target to lgm_ub_all_tests target.
  add_dependencies(
    lgm_ub_all_tests   # TARGET
    ${loop_var}_driver # TARGET DEPENDENCY
  )
endforeach(loop_var)

