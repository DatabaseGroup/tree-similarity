# Bracket notation parser tests.

# Specify list of test strings.
set(parser_test_list
  parser_size_test            # Testing tree size after parsing.
  parser_labels_test          # Testing parsed labels.
  parser_tokens_test          # Testing parsed tokens.
  parser_collection_size_test # Testing collection tree sizes after parsing (reading from file).
)

# Add custom target for building all parser's tests.
add_custom_target(parser_all_tests)

# Specify test strings in the loop.
foreach(loop_var ${parser_test_list})
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
  # Add this target to parser_all_tests target.
  add_dependencies(
    parser_all_tests   # TARGET
    ${loop_var}_driver # TARGET DEPENDENCY
  )
endforeach(loop_var)
