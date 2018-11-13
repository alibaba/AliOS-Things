NAME := device_sal_sim800

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
GLOBAL_DEFINES += DEV_SAL_SIM800

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += sal network.sal.atparser

$(NAME)_SOURCES += sim800.c
endif
GLOBAL_INCLUDES += ./
