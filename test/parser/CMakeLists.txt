# Bracket notation parser tests.

# Testing tree size after parsing.

add_executable(
  parsing_bracket_notation_size_test_driver # EXECUTABLE NAME
  parsing_bracket_notation_size_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  parsing_bracket_notation_size_test_driver # EXECUTABLE NAME
  TreeSimilarity                            # LIBRARY NAME
)

add_test(
  NAME parsing_bracket_notation_size_test           # TEST NAME
  COMMAND parsing_bracket_notation_size_test_driver # EXECUTABLE NAME
)

# Testing parsed labels.

add_executable(
  parsing_bracket_notation_labels_test_driver # EXECUTABLE NAME
  parsing_bracket_notation_labels_test.cc     # EXECUTABLE SOURCE
)

target_link_libraries(
  parsing_bracket_notation_labels_test_driver # EXECUTABLE NAME
  TreeSimilarity                            # LIBRARY NAME
)

add_test(
  NAME parsing_bracket_notation_labels_test           # TEST NAME
  COMMAND parsing_bracket_notation_labels_test_driver # EXECUTABLE NAME
)
