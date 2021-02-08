cmake_minimum_required(VERSION 3.0)
project(rds_demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

add_executable(rds_demo rds.cc)
target_link_libraries(rds_demo alibabacloud-sdk-core alibabacloud-sdk-rds)

target_link_libraries(rds_demo )

set_target_properties(rds_demo
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}rds_demo
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
