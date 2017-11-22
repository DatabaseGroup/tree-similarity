# APTED algorithm tests.

# Specify list of test strings.
set(apted_test_list
  apted_preL_to_size_test
)

# Add custom target for building all APTED's tests.
add_custom_target(apted_all_tests)

# Specify test strings in the loop.
foreach(loop_var ${apted_test_list})
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
  # Add this target to apted_all_tests target.
  add_dependencies(
    apted_all_tests   # TARGET
    ${loop_var}_driver # TARGET DEPENDENCY
  )
endforeach(loop_var)
