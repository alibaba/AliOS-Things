NAME := udataapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := uData sensor framework demo

$(NAME)_SOURCES :=   udata_example.c
$(NAME)_COMPONENTS := cli yloop drivers.sensor middleware.udata

GLOBAL_INCLUDES += ./
GLOBAL_INCLUDES += ../../../drivers/sensor/include

udata_cjson_support := 1
ifeq ($(udata_cjson_support),1)
GLOBAL_DEFINES += UDATA_CJSON_SUPPORTED
$(NAME)_COMPONENTS += utility.cjson
endif

ifeq ($(dtc),linkkit)

$(NAME)_SOURCES += linkkit/app_entry.c
$(NAME)_SOURCES += linkkit/linkkit_example_solo.c

$(NAME)_COMPONENTS += feature.linkkit netmgr utility.cjson

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

GLOBAL_INCLUDES += ./
GLOBAL_DEFINES += CONFIG_AOS_CLI
GLOBAL_DEFINES += DTC_LINKKIT

else ifeq ($(dtc),mqtt)

$(NAME)_SOURCES    += mqtt/mqtt_example.c

$(NAME)_COMPONENTS += netmgr feature.linkkit-mqtt 
GLOBAL_DEFINES     += CONFIG_AOS_CLI USE_LPTHREAD

GLOBAL_DEFINES     += DTC_MQTT
endif
