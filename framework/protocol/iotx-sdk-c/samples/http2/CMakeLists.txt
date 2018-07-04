file(GLOB C_SOURCES "*.c")
add_executable(http2-example ${C_SOURCES})
target_link_libraries(http2-example iot_sdk)
