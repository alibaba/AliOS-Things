cmake_minimum_required(VERSION 3.0)
project(cdn_demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

add_executable(cdn_demo cdn.cc)
target_link_libraries(cdn_demo alibabacloud-sdk-core alibabacloud-sdk-cdn)

target_link_libraries(cdn_demo )

set_target_properties(cdn_demo
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}cdn_demo
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
