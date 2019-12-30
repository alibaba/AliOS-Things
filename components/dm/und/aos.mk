NAME := und

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := network diagnosis provide ablity to report failure/error of network and memeory usage to cloud.
$(NAME)_SOURCES-y := src/und.c src/und_adapter.c src/und_manage.c src/und_packet.c src/und_sched.c src/und_report.c
#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS-y      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS-y      += -Wall -Werror
endif

ifeq (y,$(AOS_COMP_UND))
AOS_UND_WITH_WIFI ?= y
endif

ifeq (y,$(AOS_UND_USE_MQTT))
GLOBAL_DEFINES-y += FEATURE_UND_USE_MQTT
endif

ifeq (y,$(AOS_UND_USE_UAGENT))
GLOBAL_DEFINES-y += FEATURE_UND_USE_UAGENT
$(NAME)_COMPONENTS-y += uagent
endif

ifeq (y,$(AOS_UND_WITH_WIFI))
GLOBAL_DEFINES-y += FEATURE_UND_WITH_WIFI
$(NAME)_SOURCES-y += hal/und_wifi.c
endif

ifeq (y,$(AOS_UND_WITH_BLE))
GLOBAL_DEFINES-y += FEATURE_UND_WITH_BLE
$(NAME)_SOURCES-y += hal/und_ble.c
endif

ifeq (y,$(pvtest))
GLOBAL_DEFINES-y += PREVALIDATE_TEST
endif

$(NAME)_COMPONENTS-y += kv osal_aos yloop linkkit_sdk_c

$(NAME)_INCLUDES += include

GLOBAL_DEFINES-y += FEATURE_UND_SUPPORT
GLOBAL_DEFINES-y += AOS_UND
