##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

NAME := atparser

with_atparser ?= 1

GLOBAL_INCLUDES += ./

$(NAME)_MBINS_TYPE := kernel

$(NAME)_SOURCES := atparser.c
GLOBAL_DEFINES += AOS_ATCMD

ifeq (1,$(hdlc))
$(NAME)_SOURCES += hdlc.c
GLOBAL_DEFINES += HDLC_UART
endif

# Follow original modification, it seems needed to sleep a little bit
# due to developerkit(bk7231 wifi module inside) limited resource
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME),developerkit)
GLOBAL_DEFINES += AT_PARSER_DELAY_FLAG
endif
