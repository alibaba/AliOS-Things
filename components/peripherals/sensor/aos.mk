NAME := sensor

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := Hardware abstract layer for sensors

$(NAME)_SOURCES += \
        hal/sensor_hal.c \
        hal/sensor_drv_api.c

CONFIG_SENSOR =  $(addsuffix .c,$(CONFIG_SENSOR_DRV_NAME))
SENSOR_ALL_FILE=$(notdir $(wildcard drivers/sensor/drv/*.c))

FILTER_FILE=$(foreach defines_var,$(1),$(filter $(defines_var),$(2)))
SENSOR_SRC_FILE=$(call FILTER_FILE,$(CONFIG_SENSOR),$(SENSOR_ALL_FILE))

$(NAME)_SOURCES += $(addprefix drv/,$(SENSOR_SRC_FILE))

CONFIG_DRV_SET = $(word 1,$(CONFIG_SENSOR_DRV_NAME))

$(NAME)_COMPONENTS += vfs

ifneq ($(CONFIG_DRV_SET),)
GLOBAL_DEFINES += SENSOR_DRV_AUTO_INIT
endif

ifeq ($(AOS_SENSOR_MODBUS_ENABLE),y)
$(NAME)_SOURCES += drv/modbus_sensors/drv_modbus_sensors.c
$(NAME)_COMPONENTS  += mbmaster
GLOBAL_DEFINES += UDATA_MODBUS
endif

ifeq ($(AOS_SENSOR_INT_ENABLE),y)
GLOBAL_DEFINES += SENSOR_INT_ENABLE
endif

ifeq ($(AOS_SENSOR_I2C_ENABLE),y)
GLOBAL_DEFINES += UDATA_MEMS
GLOBAL_DEFINES += SENSOR_I2C_ENABLE
endif

ifeq ($(AOS_SENSOR_SPI_ENABLE),y)
GLOBAL_DEFINES += UDATA_MEMS
GLOBAL_DEFINES += SENSOR_SPI_ENABLE
endif

ifeq ($(AOS_SENSOR_GPS_SIMCOM_SIM868),y)
$(NAME)_COMPONENTS += atparser device_sal_sim800
$(NAME)_SOURCES += drv/drv_gps_simcom_sim868.c
GLOBAL_DEFINES += AOS_SENSOR_GPS_SIMCON_SIM868
endif

ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_INCLUDES +=  ./include
GLOBAL_DEFINES      += AOS_SENSOR
include $(SOURCE_ROOT)/components/peripherals/sensor/drv.mk
