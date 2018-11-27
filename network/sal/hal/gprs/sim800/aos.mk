NAME := device_sal_sim800

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := sal hal implmentation for sim800
GLOBAL_DEFINES += DEV_SAL_SIM800

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += network.sal.atparser

$(NAME)_SOURCES += sim800.c
endif
GLOBAL_INCLUDES += ./
