NAME := ulog

$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ulog module
$(NAME)_MBINS_TYPE := kernel

$(NAME)_COMPONENTS += osal_aos

GLOBAL_DEFINES += AOS_COMP_ULOG
#sync mode is default selected using
$(NAME)_SOURCES     := log.c ulog_init.c ulog_utility.c

$(NAME)_CFLAGS      += -Wall -Werror

ifeq ($(ULOG_CONFIG_ASYNC),y)
GLOBAL_DEFINES += ULOG_CONFIG_ASYNC
$(NAME)_SOURCES     += ulog.c
$(NAME)_SOURCES     += uring_fifo.c
$(NAME)_SOURCES     += session_fs.c
$(NAME)_SOURCES     += session_udp.c
endif

GLOBAL_INCLUDES += include

