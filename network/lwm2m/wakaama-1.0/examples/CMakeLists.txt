cmake_minimum_required(VERSION 2.8)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/bootstrap_server)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/client)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lightclient)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/server)

project(wakaama C)
