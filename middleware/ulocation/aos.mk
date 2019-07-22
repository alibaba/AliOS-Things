NAME := ulocation

$(NAME)_TYPE := framework
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := locating framework

ifeq ($(QXWZ_ENABLED),y)
$(NAME)_SOURCES += \
                   qianxun/ulocation_qxwz_socket.c \
                   qianxun/ulocation_qxwz_utils.c  \
                   qianxun/ulocation_qxwz_service.c

$(NAME)_INCLUDES += ./qianxun/
$(NAME)_PREBUILT_LIBRARY := ./qianxun/lib/$(HOST_ARCH)/qxwz.a
GLOBAL_INCLUDES += ./qianxun/include

else
$(NAME)_SOURCES += \
                   ulocation_north.c        \
                   ulocation_main.c         \
                   ulocation_south.c        \
                   hal/ulocation_hal_gprs.c \
                   hal/ulocation_hal_wifi.c \
                   hal/ulocation_hal_gps.c

$(NAME)_COMPONENTS :=

$(NAME)_INCLUDES += . include
$(NAME)_INCLUDES += ./hal
endif

GLOBAL_DEFINES += AOS_ULOCATION
