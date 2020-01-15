NAME := device_sal_m02h

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implemenation for m02h

GLOBAL_DEFINES += DEV_SAL_M02H
GLOBAL_CFLAGS += -D__SAL_DEV_NAME=m02h

$(NAME)_SOURCES += m02h.c

GLOBAL_INCLUDES += ./
