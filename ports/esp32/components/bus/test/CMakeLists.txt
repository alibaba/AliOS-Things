idf_component_register(SRCS "test_i2c_bus.c" "test_spi_bus.c"
                        INCLUDE_DIRS .
                        REQUIRES test_utils bus)