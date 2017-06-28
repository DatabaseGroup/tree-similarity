# Move the executable into other directory.
set(EXECUTABLE_OUTPUT_PATH ..)

# The set of all source files (only the cc files).
# In this case the single file could be explicitly passes to add_executable().
set(tree_similarity_sources
  tree_similarity.cc
)

# Build executable for out tool.
add_executable(${PROJECT_NAME} ${tree_similarity_sources})

# Let the compiler know to find the header files in TreeSimilarity library.
target_link_libraries(${PROJECT_NAME} TreeSimilarity)
