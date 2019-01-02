NAME := ulog

$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ulog module
$(NAME)_MBINS_TYPE := kernel

GLOBAL_DEFINES += AOS_COMP_ULOG
#sync mode is default selected using
$(NAME)_SOURCES     := log.c ulog_init.c

ifeq ($(log_async),1)
GLOBAL_DEFINES += ULOG_CONFIG_ASYNC
$(NAME)_SOURCES     += ulog.c
$(NAME)_SOURCES     += uring_fifo.c
endif

GLOBAL_INCLUDES += include

