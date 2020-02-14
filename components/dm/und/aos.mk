NAME := und

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := network diagnosis provide ablity to report failure/error of network and memeory usage to cloud.
$(NAME)_SOURCES-y := src/und.c src/und_adapter.c src/und_manage.c src/und_packet.c src/und_sched.c src/und_report.c

$(NAME)_SOURCES-$(UND_CONFIG_WITH_WIFI) += hal/und_wifi.c
$(NAME)_SOURCES-$(UND_CONFIG_WITH_BLE) += hal/und_ble.c

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS-y      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS-y      += -Wall -Werror
endif

$(NAME)_COMPONENTS-$(UND_CONFIG_USE_UAGENT) += uagent

ifeq (y,$(pvtest))
GLOBAL_DEFINES-y += PREVALIDATE_TEST
endif

$(NAME)_INCLUDES += include
GLOBAL_INCLUDES += ../../../include/dm/
