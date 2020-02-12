NAME := netmgr

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.3
$(NAME)_SUMMARY := network manager manages different types of RF.
$(NAME)_SOURCES-y := src/netmgr.c

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
$(NAME)_SOURCES-y += hal/wifi.c
$(NAME)_SOURCES-y += src/netmgr_wifi.c
endif

$(NAME)_COMPONENTS-y += kv osal_aos yloop

$(NAME)_INCLUDES += src/include
$(NAME)_INCLUDES += src

GLOBAL_INCLUDES += ../../../include/network/netmgr
GLOBAL_INCLUDES += ../../../include/network
GLOBAL_INCLUDES += ../../../include
