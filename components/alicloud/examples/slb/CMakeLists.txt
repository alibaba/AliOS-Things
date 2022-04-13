cmake_minimum_required(VERSION 3.0)
project(slb_demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

add_executable(slb_demo slb.cc)
target_link_libraries(slb_demo alibabacloud-sdk-core alibabacloud-sdk-slb)

target_link_libraries(slb_demo )

set_target_properties(slb_demo
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}slb_demo
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
