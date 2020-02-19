# component name
NAME := sensor
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := Hardware abstract layer for sensors

# source files and the folder of internal include files
$(NAME)_INCLUDES +=  ./include
$(NAME)_SOURCES += \
        hal/sensor_hal.c \
        hal/sensor_drv_api.c

# the folder of API files
GLOBAL_INCLUDES += ../../../include/peripherals/sensor

# armcc & iar without -Wall and -Werror
#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

ifeq ($(AOS_SENSOR_GPS_SIMCOM_SIM868),y)
$(NAME)_SOURCES += drv/drv_gps_simcom_sim868.c
endif

ifeq ($(SENSOR_CONFIG_MODBUS_ENABLE),y)
$(NAME)_SOURCES += drv/modbus_sensors/drv_modbus_sensors.c
endif

include $(SOURCE_ROOT)/components/peripherals/sensor/drv.mk

$(NAME)_COMPONENTS-$(AOS_SENSOR_GPS_SIMCOM_SIM868) += at device_sal_sim800
$(NAME)_COMPONENTS-$(SENSOR_CONFIG_DEPENDS_ENABLE) += vfs
$(NAME)_COMPONENTS-$(SENSOR_CONFIG_MODBUS_ENABLE) += mbmaster


