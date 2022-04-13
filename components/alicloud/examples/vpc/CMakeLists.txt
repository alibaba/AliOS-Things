cmake_minimum_required(VERSION 3.0)
project(vpc_demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

add_executable(vpc_demo vpc.cc)
target_link_libraries(vpc_demo alibabacloud-sdk-core alibabacloud-sdk-vpc)

target_link_libraries(vpc_demo )

set_target_properties(vpc_demo
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}vpc_demo
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
