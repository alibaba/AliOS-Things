NAME := ulocation

$(NAME)_TYPE := framework
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := locating framework

ifeq ($(QXWZ_ENABLED),y)
$(NAME)_SOURCES += \
                   qianxun/uloc_qxwz_socket.c \
                   qianxun/uloc_qxwz_utils.c  \
                   qianxun/uloc_qxwz_service.c

GLOBAL_INCLUDES += ./qianxun/include
GLOBAL_INCLUDES += ./qianxun/
$(NAME)_PREBUILT_LIBRARY := ./qianxun/lib/$(HOST_ARCH)/qxwz.a

else
$(NAME)_SOURCES += \
                   uloc_north.c        \
                   uloc_main.c         \
                   uloc_south.c        \
                   hal/uloc_hal_gprs.c \
                   hal/uloc_hal_wifi.c \
                   hal/uloc_hal_gps.c

$(NAME)_COMPONENTS :=

GLOBAL_INCLUDES += . include
GLOBAL_INCLUDES += ./hal
endif

GLOBAL_DEFINES += AOS_ULOCATION
