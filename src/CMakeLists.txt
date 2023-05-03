# Let CMake know about subdirectories.
add_subdirectory(command_line/)

install( DIRECTORY
  node
  label
  cost_model
  ted
  data_structures
  parser
  lookup
  join
  tree_generator
  ted_ub
  ted_lb
  json
  DESTINATION include/tree-similiarity)