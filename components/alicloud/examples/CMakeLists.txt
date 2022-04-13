cmake_minimum_required(VERSION 3.1)

cmake_policy(SET CMP0048 NEW)

project(alibabacloud-sdk-examples)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(imagesearch)
add_subdirectory(nlp)
add_subdirectory(rds)
add_subdirectory(ecs)
add_subdirectory(vpc)
add_subdirectory(slb)
add_subdirectory(cdn)
