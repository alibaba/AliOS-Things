cmake_minimum_required(VERSION 3.1)

cmake_policy(SET CMP0048 NEW)

file(STRINGS "VERSION" version)

project(alibabacloud-sdk VERSION ${version})

message(STATUS "Project version: ${PROJECT_VERSION}")

set(TARGET_OUTPUT_NAME_PREFIX "alibabacloud-sdk-" CACHE STRING "The target's output name prefix")
set(BUILD_PRODUCT "" CACHE STRING "Build by lower name of product")

option(BUILD_SHARED_LIBS  "Enable shared library" ON)
option(BUILD_UNIT_TESTS "Enable unit tests" OFF)
option(BUILD_FUNCTION_TESTS "Enable function test" OFF)

set(LIB_TYPE STATIC)

if(BUILD_SHARED_LIBS)
	set(LIB_TYPE SHARED)
	add_definitions(-DALIBABACLOUD_SHARED)
endif()

set_property(GLOBAL
	PROPERTY
	USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)

include(ExternalProject)
include(GNUInstallDirs)

add_subdirectory(3rdparty)

if(BUILD_UNIT_TESTS)
  enable_testing()
  add_subdirectory(core)
  add_subdirectory(test/core)
endif()

if(BUILD_FUNCTION_TESTS)
  enable_testing()
  add_subdirectory(core)
  add_subdirectory(cdn)
  add_subdirectory(cs)
  add_subdirectory(ecs)
  add_subdirectory(nlp)
  add_subdirectory(rds)
  add_subdirectory(slb)
  add_subdirectory(vpc)
  add_subdirectory(test/function_test/cdn)
  add_subdirectory(test/function_test/core)
  add_subdirectory(test/function_test/cs)
  add_subdirectory(test/function_test/ecs)
  add_subdirectory(test/function_test/nlp)
  add_subdirectory(test/function_test/rds)
  add_subdirectory(test/function_test/slb)
  add_subdirectory(test/function_test/vpc)
endif()

if(BUILD_PRODUCT STREQUAL "")
  message(WARNING   "Please set the value of 'BUILD_PRODUCT'")
elseif(BUILD_PRODUCT STREQUAL "core")
  add_subdirectory(core)
else()
  add_subdirectory(core)
  add_subdirectory(${BUILD_PRODUCT})
  message(STATUS   "'BUILD_PRODUCT'=${BUILD_PRODUCT}")
endif()
