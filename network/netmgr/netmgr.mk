NAME := netmgr

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := netmgr.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

net ?=  wifi
ifeq ($(net), wifi)
ifneq (,$(ssid))
$(NAME)_DEFINES += WIFI_SSID=\"$(ssid)\"
$(NAME)_DEFINES += WIFI_PWD=\"$(pwd)\"
endif
$(NAME)_SOURCES += interfaces/netmgr_wifi.c
GLOBAL_DEFINES += NET_WITH_WIFI
$(NAME)_COMPONENTS += halwifi
else
$(NAME)_SOURCES += hal/net.c
$(NAME)_SOURCES += interfaces/netmgr_net.c
GLOBAL_INCLUDES += ../include/hal/
endif

$(NAME)_COMPONENTS += kernel.fs.kv yloop kernel.hal

GLOBAL_INCLUDES += include ../../middleware/alink/protocol/os/platform/

GLOBAL_DEFINES += AOS_NETMGR
