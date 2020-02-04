NAME := ulocation

$(NAME)_TYPE := framework
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := locating framework

ifeq ($(QXWZ_ENABLED),y)
$(NAME)_SOURCES += \
                   qianxun/ulocation_qxwz_socket.c \
                   qianxun/ulocation_qxwz_utils.c  \
                   qianxun/ulocation_qxwz_service.c

$(NAME)_INCLUDES += ./qianxun/ ./qianxun/include
$(NAME)_PREBUILT_LIBRARY := ./qianxun/lib/$(HOST_ARCH)/qxwz.a
endif

ifeq ($(ULOCATION_GPS),y)
$(NAME)_SOURCES += hal/ulocation_hal_gps.c
endif

ifeq ($(ULOCATION_GPRS),y)
$(NAME)_SOURCES += hal/ulocation_hal_gprs.c
else ifeq ($(ULOCATION_WIFI),y)
$(NAME)_SOURCES += hal/ulocation_hal_wifi.c
endif

$(NAME)_SOURCES += \
                   ulocation_north.c        \
                   ulocation_main.c         \
                   ulocation_south.c        \
                   hal/ulocation_hal.c

$(NAME)_COMPONENTS := sensor cjson

$(NAME)_INCLUDES += . include
$(NAME)_INCLUDES += ./hal

GLOBAL_DEFINES += AOS_ULOCATION
