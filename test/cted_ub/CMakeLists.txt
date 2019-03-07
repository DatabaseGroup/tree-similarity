# Constrained Tree Edit Distance algorithm tests.

# Specify list of test strings.
set(cted_ub_test_list
  cted_ub_ted_test
)

# Add custom target for building all CTED tests.
add_custom_target(cted_ub_all_tests)

# Specify test strings in the loop.
foreach(loop_var ${cted_ub_test_list})
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
  # Add this target to cted_ub_all_tests target.
  add_dependencies(
    cted_ub_all_tests   # TARGET
    ${loop_var}_driver # TARGET DEPENDENCY
  )
endforeach(loop_var)

