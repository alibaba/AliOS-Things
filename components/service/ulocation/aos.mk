# component name
NAME := ulocation
# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := location component
# source files and the folder of internal include files
$(NAME)_INCLUDES += . include ./hal
$(NAME)_SOURCES += \
                   ulocation_north.c        \
                   ulocation_main.c         \
                   ulocation_south.c        \
                   hal/ulocation_hal.c

# the folder of API files
GLOBAL_INCLUDES += ../../../include/service/ulocation

ifeq ($(CONFIG_QXWZ_ENABLED),y)
$(NAME)_SOURCES += \
                   qianxun/ulocation_qxwz_socket.c \
                   qianxun/ulocation_qxwz_utils.c  \
                   qianxun/ulocation_qxwz_service.c

$(NAME)_INCLUDES += ./qianxun/ ./qianxun/include
$(NAME)_PREBUILT_LIBRARY := ./qianxun/lib/$(HOST_ARCH)/qxwz.a
endif

ifeq ($(CONFIG_ULOCATION_GPS),y)
$(NAME)_SOURCES += hal/ulocation_hal_gps.c
endif

ifeq ($(CONFIG_ULOCATION_GPRS),y)
$(NAME)_SOURCES += hal/ulocation_hal_gprs.c
else ifeq ($(CONFIG_ULOCATION_WIFI),y)
$(NAME)_SOURCES += hal/ulocation_hal_wifi.c
endif

# optional dependencies
$(NAME)_COMPONENTS-$(CONFIG_ULOCATION_GPS) += sensor cjson
$(NAME)_COMPONENTS-$(CONFIG_ULOCATION_AGPS) += cjson
