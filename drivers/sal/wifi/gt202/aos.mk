NAME := device_sal_gt202

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implementation for gt202

$(NAME)_SOURCES += gt202_sal.c

GLOBAL_DEFINES += DEV_SAL_GT202
