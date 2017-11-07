# Join tests.

# Testing naive self join results.

# Copy test cases.
file(
  COPY naive_self_join_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  naive_self_join_test_driver # EXECUTABLE NAME
  naive_self_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  naive_self_join_test_driver # EXECUTABLE NAME
  TreeSimilarity              # LIBRARY NAME
)

add_test(
  NAME naive_self_join_test           # TEST NAME
  COMMAND naive_self_join_test_driver # EXECUTABLE NAME
)
