NAME := das_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := Link Security Device Attestation example

$(NAME)_SOURCES := app_entry.c
$(NAME)_SOURCES += mqtt_das_example.c
$(NAME)_COMPONENTS := linkkit_sdk_c netmgr cjson cli das

DAS_CLIENT_CONFIG_TEST_LOOP ?= y

ifeq ($(DAS_CLIENT_CONFIG_TEST_LOOP),y)
$(NAME)_DEFINES      += TEST_LOOP
endif

$(NAME)_INCLUDES += ./

#GLOBAL_CFLAGS += -DON_DAILY

