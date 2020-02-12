NAME := device_mal_sim800

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := mal hal implmentation for sim800

GLOBAL_CFLAGS += -D__MAL_DEV_NAME=sim800

$(NAME)_SOURCES += sim800.c

GLOBAL_INCLUDES += ./

$(NAME)_COMPONENTS += at yloop
