NAME := udataapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uData sensor framework demo

$(NAME)_SOURCES :=   udata_example.c
$(NAME)_COMPONENTS := cli yloop sensor udata

GLOBAL_INCLUDES += ./

ifeq ($(dtc),linkkit)
AOS_CONFIG_DTC_LINKKIT = y
AOS_CONFIG_DTC_ENABLE = y
else ifeq ($(dtc),mqtt)
AOS_CONFIG_DTC_MQTT = y
AOS_CONFIG_DTC_ENABLE = y
endif

ifeq ($(AOS_CONFIG_DTC_LINKKIT),y)

$(NAME)_SOURCES += linkkit/app_entry.c
$(NAME)_SOURCES += linkkit/linkkit_example_solo.c

$(NAME)_COMPONENTS += feature.linkkit netmgr cjson

ifeq ($(AOS_CONFIG_LWIP_ENABLE),y)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

GLOBAL_INCLUDES += ./
GLOBAL_DEFINES += CONFIG_AOS_CLI
GLOBAL_DEFINES += DTC_LINKKIT

else ifeq ($(AOS_CONFIG_DTC_MQTT),y)

$(NAME)_SOURCES    += mqtt/mqtt_example.c

$(NAME)_COMPONENTS += netmgr feature.linkkit-mqtt
GLOBAL_DEFINES     += CONFIG_AOS_CLI USE_LPTHREAD

GLOBAL_DEFINES     += DTC_MQTT
endif
