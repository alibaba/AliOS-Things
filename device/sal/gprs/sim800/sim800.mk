NAME := device_sal_sim800

GLOBAL_DEFINES += DEV_SAL_SIM800

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += sal atparser

$(NAME)_SOURCES += sim800.c
endif
GLOBAL_INCLUDES += ./
