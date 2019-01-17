NAME := mqttapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := mqtt examples

$(NAME)_SOURCES := app_entry.c
$(NAME)_COMPONENTS := feature.linkkit-mqtt netmgr cjson cli

ifeq ($(MQTTAPP_CONFIG_CASE),rrpc)
$(NAME)_SOURCES += mqtt_example_rrpc.c
else ifeq ($(MQTTAPP_CONFIG_CASE),multithread)
$(NAME)_SOURCES += mqtt_example_multithread.c
else ifeq ($(MQTTAPP_CONFIG_CASE),presstest)
$(NAME)_SOURCES += mqtt_presstest.c
else
$(NAME)_SOURCES += mqtt_example.c
endif


ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifneq ($(MQTTAPP_CONFIG_TEST_LOOP),0)
$(NAME)_DEFINES      += TEST_LOOP
endif

GLOBAL_INCLUDES += ./

