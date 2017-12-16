NAME := device_sal_mk3060

$(NAME)_COMPONENTS += sal atparser

$(NAME)_SOURCES += mk3060.c

GLOBAL_DEFINES += DEV_SAL_MK3060
