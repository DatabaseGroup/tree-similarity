# Join tests.

# Testing naive join results.

# Copy test cases.
file(
  COPY naive_join_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  naive_join_test_driver # EXECUTABLE NAME
  naive_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  naive_join_test_driver # EXECUTABLE NAME
  TreeSimilarity              # LIBRARY NAME
)

add_test(
  NAME naive_join_test           # TEST NAME
  COMMAND naive_join_test_driver # EXECUTABLE NAME
)

# Testing tjoin results.

# Copy test cases.
file(
  COPY t_join_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  t_join_test_driver # EXECUTABLE NAME
  t_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  t_join_test_driver # EXECUTABLE NAME
  TreeSimilarity     # LIBRARY NAME
)

add_test(
  NAME t_join_test           # TEST NAME
  COMMAND t_join_test_driver # EXECUTABLE NAME
)

# Testing tangjoin results.

# Copy test cases.
file(
  COPY tang_join_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  tang_join_test_driver # EXECUTABLE NAME
  tang_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  tang_join_test_driver # EXECUTABLE NAME
  TreeSimilarity        # LIBRARY NAME
)

add_test(
  NAME tang_join_test           # TEST NAME
  COMMAND tang_join_test_driver # EXECUTABLE NAME
)

# Testing guha results.

# Copy test cases.
file(
  COPY guha_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(
  guha_test_driver # EXECUTABLE NAME
  guha_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  guha_test_driver # EXECUTABLE NAME
  TreeSimilarity   # LIBRARY NAME
)

add_test(
  NAME guha_test           # TEST NAME
  COMMAND guha_test_driver # EXECUTABLE NAME
)
