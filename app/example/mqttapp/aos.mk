NAME := mqttapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := mqtt examples

$(NAME)_SOURCES := app_entry.c 
$(NAME)_COMPONENTS := feature.linkkit-mqtt netmgr cjson cli

GLOBAL_DEFINES += CONFIG_AOS_CLI

ifeq ($(case),rrpc)
$(NAME)_SOURCES += mqtt_example_rrpc.c
else ifeq ($(case),multithread)
$(NAME)_SOURCES += mqtt_example_multithread.c
else ifeq ($(case),presstest)
$(NAME)_SOURCES += mqtt_presstest.c
else
$(NAME)_SOURCES += mqtt_example.c
endif


ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifneq ($(loop),0)
$(NAME)_DEFINES      += TEST_LOOP
endif

GLOBAL_INCLUDES += ./

