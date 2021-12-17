# Move the executable into other directory.
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Build executable for out tool.
add_executable(
  ted     # EXECUTABLE NAME
  main.cc # EXECUTABLE SOURCE
)

# Let the compiler know to find the header files in TreeSimilarity library.
target_link_libraries(
  ted            # EXECUTABLE NAME
  TreeSimilarity # LIBRARY NAME
)
