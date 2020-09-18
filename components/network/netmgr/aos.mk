NAME := netmgr

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.3.1
$(NAME)_SUMMARY := network manager manages different types of RF.

ifeq (y,$(AOS_NETMGR_WITH_CLASSIC))
$(NAME)_SOURCES-y := src/netmgr.c
else
$(NAME)_INCLUDES := include
$(NAME)_SOURCES-y := src/netmgr_conn.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS-y      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS-y      += -Wall -Werror
endif

ifeq (y,$(AOS_NET_WITH_CELLULAR))
$(NAME)_SOURCES-y += hal/cellular.c
$(NAME)_SOURCES-y += src/netmgr_cellular.c
endif

ifeq (y,$(AOS_NET_WITH_WIFI))
ifeq (y,$(AOS_NETMGR_WITH_CLASSIC))
$(NAME)_SOURCES-y += src/netmgr_wifi.c
else
$(NAME)_SOURCES-y += src/string_convert.c
$(NAME)_SOURCES-y += src/wifi_service.c
$(NAME)_COMPONENTS-y += sntp littlefs
endif
$(NAME)_SOURCES-y += hal/wifi.c
endif

ifeq (y,$(AOS_NET_WITH_BLANK))
$(NAME)_SOURCES-y += hal/net.c
$(NAME)_SOURCES-y += src/netmgr_net.c
endif

$(NAME)_COMPONENTS-y += kv osal_aos yloop

$(NAME)_INCLUDES += src/include
$(NAME)_INCLUDES += src

GLOBAL_INCLUDES += ../../../include/network/netmgr
GLOBAL_INCLUDES += ../../../include/network
GLOBAL_INCLUDES += ../../../include

# DO NOT DELETE, for RPM package
RPM_INCLUDE_DIR := network/netmgr
