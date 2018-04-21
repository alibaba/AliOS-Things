include_directories(${PROJECT_SOURCE_DIR}/layers/config/include)
include_directories(${PROJECT_SOURCE_DIR}/import/ubuntu/include/mbedtls)

set(CONFIG_TEST_C_SOURCES config_sample.c)
add_executable(comfig_sample ${CONFIG_TEST_C_SOURCES})

target_link_libraries(comfig_sample config)

