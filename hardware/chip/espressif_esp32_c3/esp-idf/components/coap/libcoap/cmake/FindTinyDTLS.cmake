# FindTinyDTLS
# -----------
#
# Find the tinyDTLS encryption library.
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``tinydtls``
#   The tinyDTLS ``tinydtls`` library, if found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``TINYDTLS_FOUND``
#   System has the tinyDTLS library.
# ``TINYDTLS_INCLUDE_DIR``
#   The tinyDTLS include directory.
# ``TINYDTLS_LIBRARIES``
#   All tinyDTLS libraries.
#
# Hints
# ^^^^^
#
# Set ``TINYDTLS_ROOT_DIR`` to the root directory of an tinyDTLS installation.

if(TINYDTLS_ROOT_DIR)
  set(_EXTRA_FIND_ARGS "NO_CMAKE_FIND_ROOT_PATH")
endif()

find_path(
  TINYDTLS_INCLUDE_DIR
  NAMES tinydtls/dtls.h
  PATH_SUFFIXES include
  HINTS ${PROJECT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${TINYDTLS_ROOT_DIR}
        ${_EXTRA_FIND_ARGS})

find_library(
  TINYDTLS_LIBRARIES
  NAMES tinydtls
  PATH_SUFFIXES lib
  HINTS ${PROJECT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${TINYDTLS_ROOT_DIR}
        ${_EXTRA_FIND_ARGS})

if(TINYDTLS_LIBRARIES)
  set(TINYDTLS_FOUND TRUE)
else()
  set(TINYDTLS_FOUND FALSE)
  if(TinyDTLS_FIND_REQUIRED)
    message(FATAL_ERROR "Tinydtls could not be found")
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  tinyDTLS
  FOUND_VAR
  TINYDTLS_FOUND
  REQUIRED_VARS
  TINYDTLS_INCLUDE_DIR
  TINYDTLS_LIBRARIES
  VERSION_VAR)

if(NOT
   TARGET
   tinydtls)
  add_library(
    tinydtls
    UNKNOWN
    IMPORTED)
  set_target_properties(
    tinydtls
    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${TINYDTLS_INCLUDE_DIR}"
               IMPORTED_LINK_INTERFACE_LANGUAGES "C"
               IMPORTED_LOCATION "${TINYDTLS_LIBRARIES}")
endif()

message(STATUS "TINYDTLS_INCLUDE_DIR: ${TINYDTLS_INCLUDE_DIR}")
message(STATUS "TINYDTLS_LIBRARIES: ${TINYDTLS_LIBRARIES}")
message(STATUS "TINYDTLS_ROOT_DIR: ${TINYDTLS_ROOT_DIR}")
