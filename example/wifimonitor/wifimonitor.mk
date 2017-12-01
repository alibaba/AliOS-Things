##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

NAME := wifimonitor
no_with_lwip := 0

$(NAME)_SOURCES += main.c wifimonitor.c
ifeq ($(LIGHT), 1)
$(NAME)_SOURCES += lightcontrol.c
$(NAME)_DEFINES += ENABLE_LIGHT
endif

LWIP ?=1
ifeq ($(LWIP), 1)
$(NAME)_COMPONENTS  += protocols.net
endif
$(NAME)_COMPONENTS += log cli netmgr protocols.mesh

$(NAME)_DEFINES += DEBUG

CONFIG_SYSINFO_APP_VERSION = APP-1.0.0-$(CURRENT_TIME)
$(info app_version:${CONFIG_SYSINFO_APP_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"
