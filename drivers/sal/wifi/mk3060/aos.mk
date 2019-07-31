NAME := device_sal_mk3060

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implementation for mk3060
$(NAME)_SOURCES += wifi_atcmd.c
GLOBAL_DEFINES += DEV_SAL_MK3060

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += atparser

$(NAME)_SOURCES += mk3060.c
endif

$(NAME)_INCLUDES += ./
