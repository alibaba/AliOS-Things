NAME := device_sal_ch395

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implemenation for ch395

GLOBAL_CFLAGS += -D__SAL_DEV_NAME=ch395

$(NAME)_SOURCES += ch395.c

GLOBAL_INCLUDES += ./

$(NAME)_COMPONENTS += sal
