NAME := device_sal_m5310a

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implmentation for m5310a
GLOBAL_DEFINES += DEV_SAL_M5310A
GLOBAL_CFLAGS += -D__SAL_DEV_NAME=m5310a

#$(NAME)_COMPONENTS += atparser
$(NAME)_SOURCES += m5310a.c
GLOBAL_INCLUDES += ./
