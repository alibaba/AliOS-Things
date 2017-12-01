NAME := netmgr

$(NAME)_TYPE := framework
ifneq (,$(ssid))
$(NAME)_DEFINES += WIFI_SSID=\"$(ssid)\"
$(NAME)_DEFINES += WIFI_PWD=\"$(pwd)\"
endif

$(NAME)_SOURCES := netmgr.c
$(NAME)_CFLAGS += -Wall -Werror

$(NAME)_COMPONENTS += modules.fs.kv yloop hal

GLOBAL_INCLUDES += include ../protocol/alink/os/platform/

GLOBAL_DEFINES += AOS_NETMGR
