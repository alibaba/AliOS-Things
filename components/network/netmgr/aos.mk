NAME := netmgr

ifeq (y,$(AOS_COMP_NETMGR_IN_SERVICE))
$(NAME)_MBINS_TYPE := share
else
$(NAME)_MBINS_TYPE := kernel
endif

$(NAME)_VERSION := 1.0.3
$(NAME)_SUMMARY := network manager manages different types of RF.

$(NAME)_COMPONENTS += activation
$(NAME)_COMPONENTS += uservice

ifeq (y,$(AOS_NETMGR_WITH_CLASSIC))
$(NAME)_SOURCES-y := src/netmgr_classic.c
else
$(NAME)_INCLUDES := include
$(NAME)_SOURCES-y += src/netmgr_conn.c
$(NAME)_SOURCES-y += src/netmgr_service.c
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS-y      += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS-y      += -Wall
endif

ifeq (y,$(AOS_NET_WITH_CELLULAR))
GLOBAL_DEFINES-y += NET_WITH_CELLULAR
$(NAME)_SOURCES-y += hal/cellular.c
$(NAME)_SOURCES-y += src/netmgr_cellular.c
$(NAME)_INCLUDES-y += ../include/hal/
endif

AOS_NET_WITH_WIFI ?= y
ifeq (y,$(AOS_NET_WITH_WIFI))
ifeq (y,$(AOS_NETMGR_WITH_CLASSIC))
$(NAME)_SOURCES-y += src/netmgr_wifi.c
else
GLOBAL_DEFINES-y += NET_WIFI_SRV
$(NAME)_SOURCES-y += src/string_convert.c
$(NAME)_SOURCES-y += src/netmgr_wifi.c
endif
endif

ifeq (y,$(AOS_NET_WITH_ETH))
$(NAME)_SOURCES-y += src/netmgr_eth.c
endif

ifeq (y,$(AOS_NET_WITH_BLANK))
$(NAME)_SOURCES-y += hal/net.c
$(NAME)_SOURCES-y += src/netmgr_net.c
$(NAME)_INCLUDES-y += ../include/hal/
endif

ifeq (y,$(pvtest))
GLOBAL_DEFINES-y += PREVALIDATE_TEST
endif

$(NAME)_COMPONENTS-y += kv osal_aos sntp

ifeq (y,$(AOS_ACTIVATION_REPORT))
$(NAME)_COMPONENTS-y += activation
endif

GLOBAL_DEFINES-y += AOS_NETMGR
GLOBAL_INCLUDES-y += network/netmgr
