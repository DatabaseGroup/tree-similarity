# All tests directories.

add_library(TestCommon INTERFACE)
target_include_directories(TestCommon INTERFACE
  common
)

add_subdirectory(parser/)
add_subdirectory(touzet/)
add_subdirectory(join/)
add_subdirectory(sed_lb/)
add_subdirectory(cted_ub/)
add_subdirectory(node/)
add_subdirectory(ted/)
add_subdirectory(lgm_ub/)

configure_file(
  CTestCustom.cmake
  ${CMAKE_BINARY_DIR}/CTestCustom.cmake
)