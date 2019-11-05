NAME := device_sal_m02h

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := sal hal implemenation for m02h

GLOBAL_DEFINES += DEV_SAL_M02H

$(NAME)_COMPONENTS += yloop

$(NAME)_COMPONENTS += atparser
$(NAME)_SOURCES += m02h.c

$(NAME)_INCLUDES += ./
