NAME := mqttapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := mqtt examples

$(NAME)_SOURCES := app_entry.c
$(NAME)_COMPONENTS := linkkit_sdk_c netmgr cjson cli

# MQTTAPP_CONFIG_TEST_LOOP ?= y

ifeq ($(MQTTAPP_CONFIG_CASE_DEFAULT),y)
$(NAME)_SOURCES += mqtt_example.c
endif

ifeq ($(MQTTAPP_CONFIG_CASE_RRPC),y)
$(NAME)_SOURCES += mqtt_example_rrpc.c
endif

ifeq ($(MQTTAPP_CONFIG_CASE_MULTITHREAD),y)
$(NAME)_SOURCES += mqtt_example_multithread.c
endif

ifeq ($(MQTTAPP_CONFIG_CASE_PRESSTEST),y)
$(NAME)_SOURCES += mqtt_presstest.c
endif

ifeq ($(MQTTAPP_CONFIG_TEST_LOOP),y)
$(NAME)_DEFINES      += TEST_LOOP
endif

GLOBAL_INCLUDES += ./

