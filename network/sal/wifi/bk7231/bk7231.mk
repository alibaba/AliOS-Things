NAME := device_sal_bk7231

$(NAME)_MBINS_TYPE := kernel
$(NAME)_SOURCES += wifi_atcmd_bk7231.c
GLOBAL_DEFINES += DEV_SAL_BK7231

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += sal network.sal.atparser

$(NAME)_SOURCES += bk7231.c
endif
GLOBAL_INCLUDES += ./
