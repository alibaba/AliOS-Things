cmake_minimum_required (VERSION 3.0)

project (bootstrap_server C)

if(DTLS)
    message(FATAL_ERROR "DTLS option is not supported." )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../../core/wakaama.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../shared/shared.cmake)

add_definitions(-DLWM2M_BOOTSTRAP_SERVER_MODE)
add_definitions(${SHARED_DEFINITIONS} ${WAKAAMA_DEFINITIONS})

include_directories (${WAKAAMA_SOURCES_DIR} ${SHARED_INCLUDE_DIRS})

SET(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/bootstrap_server.c
    ${CMAKE_CURRENT_LIST_DIR}/bootstrap_info.c
    ${CMAKE_CURRENT_LIST_DIR}/bootstrap_info.h
    )

SET(AUXILIARY_FILES
    ${CMAKE_CURRENT_LIST_DIR}/bootstrap_server.ini)

add_executable(${PROJECT_NAME} ${SOURCES} ${AUXILIARY_FILES} ${WAKAAMA_SOURCES} ${SHARED_SOURCES})

# Add WITH_LOGS to debug variant
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:WITH_LOGS>)

SOURCE_GROUP(wakaama FILES ${WAKAAMA_SOURCES})
