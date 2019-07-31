NAME := device_sal_sim800

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implmentation for sim800
GLOBAL_DEFINES += DEV_SAL_SIM800

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += atparser

$(NAME)_SOURCES += sim800.c
endif
$(NAME)_INCLUDES += ./
