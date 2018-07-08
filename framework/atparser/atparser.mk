##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

NAME := atparser

with_atparser ?= 1

GLOBAL_INCLUDES += ./

$(NAME)_SOURCES := atparser.c
GLOBAL_DEFINES += AOS_ATCMD

ifeq (1,$(hdlc))
$(NAME)_SOURCES += hdlc.c
GLOBAL_DEFINES += HDLC_UART
endif
