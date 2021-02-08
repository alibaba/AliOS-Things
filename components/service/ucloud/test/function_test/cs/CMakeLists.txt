cmake_minimum_required(VERSION 3.0)
project(demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

# note ft_build is the dir you build sdk

include_directories("../../../core/include/")
include_directories("../../../cs/include/")
link_directories(${CMAKE_SOURCE_DIR}/ft_build/lib)


add_executable(cs_ft cs_describetemplates_ut.cc)
target_link_libraries(cs_ft alibabacloud-sdk-core alibabacloud-sdk-cs)

target_link_libraries(cs_ft core gtest gmock_main)

set_target_properties(cs_ft
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}cs_ft
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_test(NAME cs_ft COMMAND cs_ft)
