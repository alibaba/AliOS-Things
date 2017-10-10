##
 # Copyright (C) 2015-2017 Alibaba Group Holding Limited
##

NAME := wifimonitor

$(NAME)_SOURCES += main.c wifimonitor.c lightcontrol.c

$(NAME)_COMPONENTS += log cli ywss gateway

$(NAME)_DEFINES += DEBUG

CONFIG_SYSINFO_APP_VERSION = APP-1.0.0-$(CURRENT_TIME)
$(info app_version:${CONFIG_SYSINFO_APP_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"
