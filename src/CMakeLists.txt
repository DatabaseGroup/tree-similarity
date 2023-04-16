# Let CMake know about subdirectories.
add_subdirectory(command_line/)

install(
  DIRECTORY .
  DESTINATION include/tree-similiarity)