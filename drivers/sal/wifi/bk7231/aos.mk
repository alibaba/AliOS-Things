NAME := device_sal_bk7231

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implemenation for bk7231
$(NAME)_SOURCES += wifi_atcmd_bk7231.c
GLOBAL_DEFINES += DEV_SAL_BK7231

$(NAME)_COMPONENTS += yloop

ifneq (1, $(at_adapter))
$(NAME)_COMPONENTS += atparser

$(NAME)_SOURCES += bk7231.c
endif

$(NAME)_INCLUDES += ./
