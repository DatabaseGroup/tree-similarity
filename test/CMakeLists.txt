# All tests directories.

add_library(TestCommon INTERFACE)
target_include_directories(TestCommon INTERFACE
  common
)

add_subdirectory(parser/)
add_subdirectory(zhang_shasha/)
add_subdirectory(touzet/)
add_subdirectory(join/)
add_subdirectory(greedy_ub/)
add_subdirectory(apted/)
add_subdirectory(sed_lb/)
add_subdirectory(cted_ub/)
add_subdirectory(node/)
add_subdirectory(ted/)