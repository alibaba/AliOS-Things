set(SOURCES "i2c_manager/i2c_manager.c")
set(INCLUDES "i2c_manager")

if (CONFIG_I2C_MANAGER_I2CDEV)
    list(APPEND SOURCES "i2cdev/i2cdev.c")
    list(APPEND INCLUDES "i2cdev")
endif()

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS ${INCLUDES}
)
