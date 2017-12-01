##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

NAME := atapp

$(NAME)_SOURCES := atapp.c

$(NAME)_COMPONENTS += cli atparser netmgr

ifeq ($(at_adapter),1)
$(NAME)_COMPONENTS += at_adapter
LWIP ?=1
endif

with_atparser ?= 1

GLOBAL_DEFINES += AOS_NO_WIFI

ifeq (1,${BINS})
GLOBAL_CFLAGS += -DSYSINFO_OS_BINS
endif
CURRENT_TIME = $(shell /bin/date +%Y%m%d.%H%M)
CONFIG_SYSINFO_APP_VERSION = APP-1.0.0-$(CURRENT_TIME)
$(info app_version:${CONFIG_SYSINFO_APP_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"
