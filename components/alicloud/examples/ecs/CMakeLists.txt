cmake_minimum_required(VERSION 3.0)
project(ecs_demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

add_executable(ecs_demo ecs.cc)
target_link_libraries(ecs_demo alibabacloud-sdk-core alibabacloud-sdk-ecs)

target_link_libraries(ecs_demo)

set_target_properties(ecs_demo
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}ecs_demo
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
