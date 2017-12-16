NAME := netmgr

$(NAME)_TYPE := framework
ifneq (,$(ssid))
$(NAME)_DEFINES += WIFI_SSID=\"$(ssid)\"
$(NAME)_DEFINES += WIFI_PWD=\"$(pwd)\"
endif

$(NAME)_SOURCES := netmgr.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_COMPONENTS += modules.fs.kv yloop hal

GLOBAL_INCLUDES += include ../protocol/alink/os/platform/

GLOBAL_DEFINES += AOS_NETMGR
