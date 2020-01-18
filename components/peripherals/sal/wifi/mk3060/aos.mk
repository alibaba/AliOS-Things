NAME := device_sal_mk3060

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := sal hal implementation for mk3060/mk3080

GLOBAL_DEFINES += DEV_SAL_MK3060
GLOBAL_CFLAGS += -D__SAL_DEV_NAME=mk3060

$(NAME)_SOURCES += mk3060.c \
                   wifi_atcmd_mk3060.c

GLOBAL_INCLUDES += .
