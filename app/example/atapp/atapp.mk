##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

NAME := atapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := atapp.c

$(NAME)_COMPONENTS += cli network.sal.atparser netmgr

ifeq ($(at_adapter),1)
$(NAME)_COMPONENTS += at_adapter
LWIP ?=1
endif

with_atparser ?= 1

GLOBAL_DEFINES += AOS_NO_WIFI

GLOBAL_INCLUDES += ./
