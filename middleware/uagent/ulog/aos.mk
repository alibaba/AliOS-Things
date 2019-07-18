NAME := ulog

$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := ulog module
$(NAME)_MBINS_TYPE := kernel

$(NAME)_COMPONENTS += osal_aos

GLOBAL_DEFINES += AOS_COMP_ULOG
#sync mode is default selected using
$(NAME)_SOURCES     := ulog.c ulog_init.c ulog_utility.c

$(NAME)_CFLAGS      += -Wall -Werror
$(NAME)_CFLAGS      += -DMOD_VER=\"$($(NAME)_VERSION)\"
ifeq ($(ULOG_CONFIG_ASYNC),y)
GLOBAL_DEFINES += ULOG_CONFIG_ASYNC
$(NAME)_SOURCES     += ulog_async.c
$(NAME)_SOURCES     += ulog_ring_fifo.c
ifeq ($(ULOG_CONFIG_POP_FS),y)
$(NAME)_SOURCES     += ulog_session_file.c
$(NAME)_SOURCES     += ulog_fs_cfg.c
endif
$(NAME)_SOURCES     += ulog_session_udp.c
endif

GLOBAL_INCLUDES += include

