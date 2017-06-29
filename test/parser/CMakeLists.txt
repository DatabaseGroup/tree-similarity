add_executable(
  parsing_bracket_notation_size_test_driver
  parsing_bracket_notation_size_test.cc
)

target_link_libraries(
  parsing_bracket_notation_size_test_driver
  TreeSimilarity
)

add_test(
  NAME parsing_bracket_notation_size_test
  COMMAND parsing_bracket_notation_size_test_driver
)
