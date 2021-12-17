# TED join algorithms correctness tests.

# List of tests and test string prefixes.
set(ted_test_list
  naivejoin
  tjoin
  tangjoin
  guhajoin
  bbjoin
  hjoin
  dhjoin
  lhjoin
  ldhjoin
)

# Test driver name.
set(
  test_driver_name
  ted_join_correctness_test_driver
)

add_executable(
  ${test_driver_name}  # EXECUTABLE NAME
  ted_join_test.cc     # EXECUTABLE SOURCE
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
set( data_20 join_test_data_20.txt )
set( res_20 join_test_results_20.txt )
set( data_129 join_test_data_129.txt )
set( res_129 join_test_results_129.txt )
#set( memcheck_input join_memcheck_data.txt )
file(
  COPY        ${data_20} ${res_20} ${data_129} ${res_129}
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# Create correctness tests in a loop. Use the same executable.
foreach(loop_var ${ted_test_list})
  add_test(
    NAME    ted_join_correctness_test_${loop_var}_20 # TEST NAME
    COMMAND ${test_driver_name}      # EXECUTABLE NAME
    ${loop_var} ${data_20} ${res_20} # ARGUMENTS
  )
  add_test(
    NAME    ted_join_correctness_test_${loop_var}_129 # TEST NAME
    COMMAND ${test_driver_name}      # EXECUTABLE NAME
    ${loop_var} ${data_129} ${res_129} # ARGUMENTS
  )
endforeach(loop_var)
