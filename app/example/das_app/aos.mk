NAME := das_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Link Security Device Attestation example

$(NAME)_SOURCES := app_entry.c
$(NAME)_SOURCES += mqtt_das_example.c
$(NAME)_COMPONENTS := linkkit_sdk_c netmgr cjson cli das

MQTTAPP_CONFIG_TEST_LOOP ?= 1
loop ?= $(MQTTAPP_CONFIG_TEST_LOOP)

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifneq ($(loop),0)
$(NAME)_DEFINES      += TEST_LOOP
endif

GLOBAL_INCLUDES += ./

#GLOBAL_CFLAGS += -DON_DAILY

