NAME := ulog

$(NAME)_VERSION := 0.0.1.0
$(NAME)_SUMMARY := ulog module
$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES     := log.c
ifeq ($(log_async),1)
GLOBAL_DEFINES += CONFIG_ULOG
$(NAME)_SOURCES     += ulog.c
$(NAME)_SOURCES     += uring_fifo.c
endif

GLOBAL_INCLUDES += include

