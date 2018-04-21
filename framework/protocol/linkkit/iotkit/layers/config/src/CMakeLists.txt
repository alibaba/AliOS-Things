include_directories(${PROJECT_SOURCE_DIR}/layers/config/include)
include_directories(${PROJECT_SOURCE_DIR}/hal-impl/refs)

#file(GLOB C_SOURCES "*.c" ${PROJECT_SOURCE_DIR}/hal-impl/refs/linux_hardware.c)
file(GLOB C_SOURCES "*.c")
add_library(config STATIC ${C_SOURCES})

target_link_libraries(config ilop_iotkit ilop_utils)

