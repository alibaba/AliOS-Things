NAME := device_sal_ec20_usb

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := sal hal implemenation for ec20

GLOBAL_CFLAGS += -D__SAL_DEV_NAME=ec20_usb

$(NAME)_SOURCES += ec20.c

GLOBAL_INCLUDES += ./

$(NAME)_COMPONENTS += at usb
