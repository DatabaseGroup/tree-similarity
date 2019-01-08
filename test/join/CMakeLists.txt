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

# Copy test cases.
file(
  COPY join_test_data.txt
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

# Testing tjoin results.

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

# Testing degree histogram results.

add_executable(
  dh_join_test_driver # EXECUTABLE NAME
  dh_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  dh_join_test_driver # EXECUTABLE NAME
  TreeSimilarity      # LIBRARY NAME
)

add_test(
  NAME dh_join_test           # TEST NAME
  COMMAND dh_join_test_driver # EXECUTABLE NAME
)

# Testing leaf distance histogram results.

add_executable(
  ldh_join_test_driver # EXECUTABLE NAME
  ldh_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  ldh_join_test_driver # EXECUTABLE NAME
  TreeSimilarity       # LIBRARY NAME
)

add_test(
  NAME ldh_join_test           # TEST NAME
  COMMAND ldh_join_test_driver # EXECUTABLE NAME
)

# Testing label histogram results.

add_executable(
  lh_join_test_driver # EXECUTABLE NAME
  lh_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  lh_join_test_driver # EXECUTABLE NAME
  TreeSimilarity       # LIBRARY NAME
)

add_test(
  NAME lh_join_test           # TEST NAME
  COMMAND lh_join_test_driver # EXECUTABLE NAME
)

# Testing binary branches results.

add_executable(
  bb_join_test_driver # EXECUTABLE NAME
  bb_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  bb_join_test_driver # EXECUTABLE NAME
  TreeSimilarity       # LIBRARY NAME
)

add_test(
  NAME bb_join_test           # TEST NAME
  COMMAND bb_join_test_driver # EXECUTABLE NAME
)

# Testing histogram results.

add_executable(
  histo_join_test_driver # EXECUTABLE NAME
  histo_join_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  histo_join_test_driver # EXECUTABLE NAME
  TreeSimilarity         # LIBRARY NAME
)

add_test(
  NAME histo_join_test           # TEST NAME
  COMMAND histo_join_test_driver # EXECUTABLE NAME
)
