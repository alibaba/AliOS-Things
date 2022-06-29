cmake_minimum_required(VERSION 3.1)

project(hydrogen LANGUAGES C)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

string(TOUPPER "${PROJECT_NAME}" setting_prefix)
function(get_setting setting_name setting_type setting_description)
  string(TOUPPER "${setting_prefix}_${setting_name}" setting_external_name)
  set("${setting_external_name}" "" CACHE "${setting_type}" "${setting_description}")
  set("${setting_name}" "${${setting_external_name}}" PARENT_SCOPE)
endfunction()

# Project files

set(source_files
    "${PROJECT_NAME}.c"
    "impl/common.h"
    "impl/core.h"
    "impl/gimli-core.h"
    "impl/gimli-core/portable.h"
    "impl/gimli-core/sse2.h"
    "impl/hash.h"
    "impl/${PROJECT_NAME}_p.h"
    "impl/kdf.h"
    "impl/kx.h"
    "impl/pwhash.h"
    "impl/random.h"
    "impl/secretbox.h"
    "impl/sign.h"
    "impl/x25519.h")

set(header_files "${PROJECT_NAME}.h")

set(test_files "tests/tests.c")

set(arduino_files "library.properties")

# Compile options

get_setting(target_arch STRING "Target system architecture (fed to the compiler's -march=XXX).")
if(NOT target_arch AND NOT CMAKE_CROSSCOMPILING)
  set(target_arch native)
endif()

get_setting(target_device STRING "Target device identifier (defines HYDRO_TARGET_DEVICE_XXX).")

set(compile_options
    # --- GNU, Clang ---
    $<$<OR:$<C_COMPILER_ID:AppleClang>,$<C_COMPILER_ID:Clang>,$<C_COMPILER_ID:GNU>>:
    # ---- Definitions ----
    $<$<BOOL:${target_device}>:-DHYDRO_TARGET_DEVICE_${target_device}>
    # ---- Optimizations ----
    -Os $<$<BOOL:${target_arch}>:-march=${target_arch}> -fno-exceptions
    # ---- Warnings ----
    -Wall
    -Wextra
    -Wmissing-prototypes
    -Wdiv-by-zero
    -Wbad-function-cast
    -Wcast-align
    -Wcast-qual
    -Wfloat-equal
    -Wmissing-declarations
    -Wnested-externs
    -Wno-unknown-pragmas
    -Wpointer-arith
    -Wredundant-decls
    -Wstrict-prototypes
    -Wswitch-enum
    -Wno-type-limits
    >
    # --- MSVC ---
    $<$<C_COMPILER_ID:MSVC>:
    # ---- Definitions ----
    $<$<BOOL:${target_device}>:/DHYDRO_TARGET_DEVICE_${target_device}>
    # ---- Optimizations ----
    /Os /EHsc
    # ---- Warnings ----
    /W4
    /wd4197 # * suppress warning "top-level volatile in cast is ignored"
    /wd4146 # * suppress warning "unary minus operator applied to unsigned type, result still
            #   unsigned"
    /wd4310 # * suppress warning "cast truncates constant value"
    >)

# Prefix project files with the project root

# Main library

add_library("${PROJECT_NAME}")
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_sources("${PROJECT_NAME}" PRIVATE ${source_files})

target_include_directories("${PROJECT_NAME}"
                           PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
                                  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_options("${PROJECT_NAME}" PRIVATE ${compile_options})

# Installation

set(targets_export_name "${PROJECT_NAME}-targets")

install(TARGETS "${PROJECT_NAME}"
        EXPORT "${targets_export_name}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(FILES ${header_files} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# CMake find_package() support

set(install_config_dir "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")

set(targets_export_file_name "${targets_export_name}.cmake")
set(targets_export_file "${PROJECT_BINARY_DIR}/${targets_export_file_name}")

install(EXPORT "${targets_export_name}"
        FILE "${targets_export_file_name}"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "${install_config_dir}")

set(config_file_name "${PROJECT_NAME}-config.cmake")
set(config_template_file "${PROJECT_SOURCE_DIR}/cmake/${config_file_name}.in")
set(config_file "${PROJECT_BINARY_DIR}/${config_file_name}")

configure_package_config_file("${config_template_file}" "${config_file}"
                              INSTALL_DESTINATION "${install_config_dir}")

install(FILES "${config_file}" DESTINATION "${install_config_dir}")

export(EXPORT "${targets_export_name}" FILE "${targets_export_file}" NAMESPACE "${PROJECT_NAME}::")

export(PACKAGE "${PROJECT_NAME}")

# Tests

set(tests_executable "${PROJECT_NAME}-tests")
set(tests_run_target "${PROJECT_NAME}-run-tests")
set(tests_run_file "${PROJECT_BINARY_DIR}/${tests_run_target}.done")

enable_testing()
add_executable("${tests_executable}" ${test_files})
target_compile_options("${tests_executable}" PRIVATE ${compile_options})
target_link_libraries("${tests_executable}" "${PROJECT_NAME}")
add_test(NAME "${tests_executable}" COMMAND "${tests_executable}")

if(CMAKE_CROSSCOMPILING)
  # Disable tests executable by default when cross-compiling (as it will fail to build when, e.g.,
  # cross-compiling for Arduino/AVR).
  set_target_properties("${tests_executable}"
                        PROPERTIES
                        EXCLUDE_FROM_ALL 1
                        EXCLUDE_FROM_DEFAULT_BUILD 1)
else()
  # Otherwise, auto-run the tests on build.
  add_custom_command(OUTPUT "${tests_run_file}"
                     DEPENDS "${tests_executable}"
                     COMMAND "${CMAKE_COMMAND}"
                     ARGS -E remove "${tests_run_file}"
                     COMMAND "${CMAKE_CTEST_COMMAND}"
                     ARGS -C $<CONFIGURATION> --output-on-failure
                     COMMAND "${CMAKE_COMMAND}"
                     ARGS -E touch "${tests_run_file}"
                     WORKING_DIRECTORY "${PROJECT_BINARY_DIR}"
                     VERBATIM)
  add_custom_target("${tests_run_target}" ALL DEPENDS "${tests_run_file}" VERBATIM)
endif()

# Generate Arduino package

set(arduino_package_file "${PROJECT_BINARY_DIR}/hydrogen-crypto.zip")

# Use the relative versions of the file path lists or else the full paths will end up in the
# generated archive.
add_custom_command(OUTPUT "${arduino_package_file}"
                   COMMAND "${CMAKE_COMMAND}"
                   ARGS -E
                        tar
                        cf
                        "${arduino_package_file}"
                        --format=zip
                        --
                        ${source_files}
                        ${header_files}
                        ${arduino_files}
                   WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                   VERBATIM)

add_custom_target("${PROJECT_NAME}-arduino-package" DEPENDS "${arduino_package_file}" VERBATIM)
