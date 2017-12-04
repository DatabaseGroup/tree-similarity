# All tests directories.

add_library(TestCommon INTERFACE)
target_include_directories(TestCommon INTERFACE
  common
)

add_subdirectory(parser/)
add_subdirectory(zhang_shasha/)
add_subdirectory(touzet/)
add_subdirectory(join/)
add_subdirectory(apted/)
