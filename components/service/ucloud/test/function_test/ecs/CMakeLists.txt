cmake_minimum_required(VERSION 3.0)
project(demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

# note ft_build is the dir you build sdk

include_directories("../../../core/include/")
include_directories("../../../ecs/include/")
link_directories(${CMAKE_SOURCE_DIR}/ft_build/lib)


add_executable(ecs_ft ecs_describeInstances_ft.cc)
target_link_libraries(ecs_ft alibabacloud-sdk-core alibabacloud-sdk-ecs)

target_link_libraries(ecs_ft core gtest gmock_main)

set_target_properties(ecs_ft
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}ecs_ft
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(NAME ecs_ft COMMAND ecs_ft)
