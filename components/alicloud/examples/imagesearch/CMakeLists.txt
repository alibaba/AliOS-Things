cmake_minimum_required(VERSION 3.0)
project(imagesearch_demo)
set(CMAKE_CXX_STANDARD 11)
if(CMAKE_HOST_WIN32)
    include_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\include")
    link_directories("C:\\Program Files (x86)\\alibabacloud-sdk\\lib")
endif()

add_executable(imagesearch_demo imagesearch.cc)
target_link_libraries(imagesearch_demo alibabacloud-sdk-core)

target_link_libraries(imagesearch_demo )

set_target_properties(imagesearch_demo
  PROPERTIES
  OUTPUT_NAME ${TARGET_OUTPUT_NAME_PREFIX}imagesearch_demo
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(COPY 1.jpg 2.jpg DESTINATION ${CMAKE_BINARY_DIR}/bin)
