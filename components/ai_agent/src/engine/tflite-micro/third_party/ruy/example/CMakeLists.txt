# This file is generated (whence no license header). Do not edit!
# To regenerate, run:
#   cmake/bazel_to_cmake.sh

ruy_cc_binary(
  NAME
    ruy_example_example
  SRCS
    example.cc
  DEPS
    ruy
)

ruy_cc_binary(
  NAME
    ruy_example_parametrized_example
  SRCS
    parametrized_example.cc
  DEPS
    ruy
)

ruy_add_all_subdirs()
