NAME := device_sal_athost

$(NAME)_SOURCES += wifi_atcmd.c
GLOBAL_DEFINES += DEV_SAL_ATHOST

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += sal network.sal.atparser

$(NAME)_SOURCES += athost.c
endif

GLOBAL_INCLUDES += .
