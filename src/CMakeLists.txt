# Let CMake know about subdirectories.
add_subdirectory(command_line/)


# Define install target (used by VCPKG)
# install and export target
#install(
#  TARGETS TreeSimilarity
#  EXPORT TreeSimilarity-targets)

install(
  DIRECTORY .
  DESTINATION include/tree-similiarity)

#install(
#  EXPORT TreeSimilarity-targets
#  FILE tree-similiarity-config.cmake
#  NAMESPACE tree-similiarity::
#  DESTINATION ${tree_similiarity_INSTALL_CMAKE_DIR}/tree-similiarity)
  
#install(
#  FILES ${CMAKE_CURRENT_BINARY_DIR}/tree-similiarity-config-version.cmake
#  DESTINATION ${tree_similiarity_INSTALL_CMAKE_DIR}/tree-similiarity)

