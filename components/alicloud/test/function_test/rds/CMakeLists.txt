cmake_minimum_required(VERSION 3.0)
project(demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

# note ft_build is the dir you build sdk
include_directories("../../../core/include/")
include_directories("../../../rds/include/")
link_directories(${CMAKE_SOURCE_DIR}/ft_build/lib)

add_executable(rds_ft rds_describeDBInstances_ft.cc)
target_link_libraries(rds_ft alibabacloud-sdk-core alibabacloud-sdk-rds)

target_link_libraries(rds_ft core gtest gmock_main)

set_target_properties(rds_ft
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}rds_ft
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(NAME rds_ft COMMAND rds_ft)
