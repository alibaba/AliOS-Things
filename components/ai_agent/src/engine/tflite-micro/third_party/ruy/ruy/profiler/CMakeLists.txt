# This file is generated (whence no license header). Do not edit!
# To regenerate, run:
#   cmake/bazel_to_cmake.sh

if(${RUY_PROFILER})
  set(ruy_profiler_0_RUY_PROFILER "RUY_PROFILER")
else()
  set(ruy_profiler_0_RUY_PROFILER "")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
  set(ruy_profiler_1_pthread "")
else()
  set(ruy_profiler_1_pthread "-pthread")
endif()

ruy_cc_library(
  NAME
    ruy_profiler_instrumentation
  SRCS
    instrumentation.cc
  HDRS
    instrumentation.h
  DEFINES
    ${ruy_profiler_0_RUY_PROFILER}
  LINKOPTS
    ${ruy_profiler_1_pthread}
  PUBLIC
)

ruy_cc_library(
  NAME
    ruy_profiler_profiler
  SRCS
    profiler.cc
    treeview.cc
  HDRS
    profiler.h
    treeview.h
  LINKOPTS
    ${ruy_profiler_1_pthread}
  PUBLIC
  DEPS
    ruy_profiler_instrumentation
)

ruy_cc_library(
  NAME
    ruy_profiler_test_instrumented_library
  TESTONLY
  SRCS
    test_instrumented_library.cc
  HDRS
    test_instrumented_library.h
  DEPS
    ruy_profiler_instrumentation
)

ruy_cc_test(
  NAME
    ruy_profiler_test
  SRCS
    test.cc
  LINKOPTS
    ${ruy_profiler_1_pthread}
  DEPS
    ruy_profiler_profiler
    ruy_profiler_test_instrumented_library
    ruy_gtest_wrapper
)

ruy_add_all_subdirs()
