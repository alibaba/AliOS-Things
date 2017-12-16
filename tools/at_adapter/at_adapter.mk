##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

NAME := at_adapter

GLOBAL_INCLUDES += ./

$(NAME)_SOURCES := at_adapter.c

$(NAME)_COMPONENTS += atparser

LWIP ?= 1
ifeq ($(LWIP), 1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
with_lwip := 1
endif

GLOBAL_DEFINES += AOS_AT_ADAPTER
