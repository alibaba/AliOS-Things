# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_policy(SET CMP0012 NEW)
cmake_policy(SET CMP0048 NEW)
project(ruy CXX)
cmake_minimum_required(VERSION 3.13)  # Copied from IREE
set(CMAKE_CXX_STANDARD 14)



if (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  set(RUY_IS_TOPLEVEL TRUE)
  set(RUY_MINIMAL_BUILD_DEFAULT_VALUE OFF)
else()
  set(RUY_IS_TOPLEVEL FALSE)
  set(RUY_MINIMAL_BUILD_DEFAULT_VALUE ON)
endif()

option(RUY_MINIMAL_BUILD "Disable ruy's tests, examples, etc. Build only ruy public libraries." ${RUY_MINIMAL_BUILD_DEFAULT_VALUE})
if (NOT RUY_MINIMAL_BUILD)
  enable_testing()
endif()

option(RUY_PROFILER "Enable ruy's built-in profiler (harms performance)" OFF)

include(cmake/ruy_add_all_subdirs.cmake)
include(cmake/ruy_cc_library.cmake)
include(cmake/ruy_cc_binary.cmake)
include(cmake/ruy_cc_test.cmake)

# Skip cpuinfo if it was already generated, which can happen when ruy is
# a subdirectory in a wider project that already uses cpuinfo.
if (NOT TARGET cpuinfo)
  # Test if the third_party/cpuinfo submodule was checked out before
  # adding that subdirectory, so we can do more helpful things below in the
  # else() block when it's not.
  set(RUY_CPUINFO_CMAKELISTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/cpuinfo/CMakeLists.txt")
  if (EXISTS "${RUY_CPUINFO_CMAKELISTS_FILE}")
    # Disabling cpuinfo's tests and benchmarks to prevent a copy of its
    # googletest dependency getting downloaded into a 'deps' directory in the
    # source tree!
    set(CPUINFO_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
    set(CPUINFO_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    set(CPUINFO_BUILD_MOCK_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory("third_party/cpuinfo" EXCLUDE_FROM_ALL)
  else()
    # third_party/cpuinfo is not checked out. That could be intentional when
    # ruy is a subdirectory in a wider project that is already providing
    # the cpuinfo target. Maybe that wider project's CMakeLists is ordered
    # in such a way that cpuinfo gets generated after ruy. In that case,
    # it's helpful that we continue silently. In the worst case if the cpuinfo
    # target never gets defined, ruy will fail to compile.
    # On the other hand, if ruy is the top-level project here (not part of a
    # wider project) then nothing will define the cpuinfo target for us,
    # so we will definitely fail to compile, so we may as well fail right here.
    if (RUY_IS_TOPLEVEL)
      message(FATAL_ERROR "This file does not exist:\n${RUY_CPUINFO_CMAKELISTS_FILE}\n"
                    "That typically means that the git submodules of the ruy "
                    "repository haven't been checked out. Try this in the ruy "
                    "git directory:\n  git submodule update --init")
    endif()
  endif()
endif()

# googletest is only needed for tests. Projects embedding ruy as a subdirectory
# and not needing to build ruy tests may proceed without a local checkout of
# third_party/googletest.
if (NOT RUY_MINIMAL_BUILD
    AND NOT TARGET gtest
    AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/CMakeLists.txt")
  add_subdirectory("third_party/googletest" EXCLUDE_FROM_ALL)
endif()

add_subdirectory("ruy")

if (NOT RUY_MINIMAL_BUILD)
  add_subdirectory("example")
endif()
