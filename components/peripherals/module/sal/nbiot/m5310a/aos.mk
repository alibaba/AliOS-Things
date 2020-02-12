NAME := device_sal_m5310a

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implmentation for m5310a

GLOBAL_CFLAGS += -D__SAL_DEV_NAME=m5310a

$(NAME)_SOURCES += m5310a.c

GLOBAL_INCLUDES += ./

$(NAME)_COMPONENTS += at yloop
