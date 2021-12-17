# JSON label tests.

# Specify list of test strings.
set(jedi_baseline_test_list
  jedi_baseline_test
)

# Add custom target for building all label intersection tests.
add_custom_target(jedi_baseline_all_tests)

# Specify test strings in the loop.
foreach(loop_var ${jedi_baseline_test_list})
  file(
    COPY        ${loop_var}_data.txt ${loop_var}_results.txt
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
    ${loop_var}_data.txt ${loop_var}_results.txt # ARGUMENTS
  )
  # Add this target to li_lb_all_tests target.
  add_dependencies(
    jedi_baseline_all_tests   # TARGET
    ${loop_var}_driver # TARGET DEPENDENCY
  )
endforeach(loop_var)

