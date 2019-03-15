NAME := netmgr

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := network manager manages different types of RF.
$(NAME)_SOURCES-y := netmgr.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS-y      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS-y      += -Wall -Werror
endif

ifeq (y,$(AOS_NET_WITH_CELLULAR))
GLOBAL_DEFINES-y += NET_WITH_CELLULAR
$(NAME)_SOURCES-y += hal/cellular.c
$(NAME)_SOURCES-y += interfaces/netmgr_cellular.c
GLOBAL_INCLUDES-y += ../include/hal/
endif

AOS_NET_WITH_WIFI ?= y
ifeq (y,$(AOS_NET_WITH_WIFI))
$(NAME)_SOURCES-y += interfaces/netmgr_wifi.c
GLOBAL_DEFINES-y += NET_WITH_WIFI
$(NAME)_SOURCES-y += hal/wifi.c
endif

ifeq (y,$(AOS_NET_WITH_BLANK))
$(NAME)_SOURCES-y += hal/net.c
$(NAME)_SOURCES-y += interfaces/netmgr_net.c
GLOBAL_INCLUDES-y += ../include/hal/
endif

ifeq (y,$(pvtest))
GLOBAL_DEFINES-y += PREVALIDATE_TEST
endif

$(NAME)_COMPONENTS-y += kv osal_aos yloop

GLOBAL_INCLUDES-y += include

GLOBAL_DEFINES-y += AOS_NETMGR
