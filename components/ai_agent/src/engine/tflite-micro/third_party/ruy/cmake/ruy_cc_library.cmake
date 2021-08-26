# Copyright 2019 Google LLC
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

# Forked from IREE's iree_cc_library.cmake.

include(CMakeParseArguments)
include(cmake/ruy_include_directories.cmake)

# ruy_cc_library()
#
# CMake function to imitate Bazel's cc_library rule.
function(ruy_cc_library)
  cmake_parse_arguments(
    _RULE
    "PUBLIC;TESTONLY"
    "NAME"
    "HDRS;SRCS;COPTS;DEFINES;LINKOPTS;DEPS"
    ${ARGN}
  )

  if(_RULE_TESTONLY AND RUY_MINIMAL_BUILD)
    return()
  endif()

  set(_NAME "${_RULE_NAME}")

  # Check if this is a header-only library.
  if("${_RULE_SRCS}" STREQUAL "")
    set(_RULE_IS_INTERFACE 1)
  else()
    set(_RULE_IS_INTERFACE 0)
  endif()

  if(_RULE_IS_INTERFACE)
    # Generating a header-only library.
    add_library(${_NAME} INTERFACE)
    target_include_directories(${_NAME}
      INTERFACE
        "${PROJECT_SOURCE_DIR}"
    )
    target_link_libraries(${_NAME}
      INTERFACE
        ${_RULE_DEPS}
        ${_RULE_LINKOPTS}
    )
    target_compile_definitions(${_NAME}
      INTERFACE
        ${_RULE_DEFINES}
    )
  else()
    # Generating a static binary library.
    add_library(${_NAME} STATIC "")
    target_sources(${_NAME}
      PRIVATE
        ${_RULE_SRCS}
        ${_RULE_HDRS}
    )
    ruy_include_directories(${_NAME} "${_RULE_DEPS}")
    target_compile_options(${_NAME}
      PRIVATE
        ${_RULE_COPTS}
    )
    target_link_libraries(${_NAME}
      PUBLIC
        ${_RULE_DEPS}
      PRIVATE
        ${_RULE_LINKOPTS}
    )
    target_compile_definitions(${_NAME}
      PUBLIC
        ${_RULE_DEFINES}
    )
  endif()
endfunction()
