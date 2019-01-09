NAME := athostapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := An example for receiving and processing AT command sets.
$(NAME)_SOURCES := athostapp.c
$(NAME)_COMPONENTS := athost netmgr cli yloop

ATHOSTAPP_CONFIG_NO_ATPARSER ?= n
ATHOSTAPP_CONFIG_NO_MQTTREPORT ?= n

ifeq ($(ATHOSTAPP_CONFIG_NO_ATPARSER),n)
$(NAME)_COMPONENTS += atparser
endif

ifeq ($(ATHOSTAPP_CONFIG_NO_MQTTREPORT),y)
GLOBAL_DEFINES += ATHOST_MQTT_REPORT_DISBALED
endif
GLOBAL_DEFINES += DEBUG

$(NAME)_INCLUDES := ./

GLOBAL_INCLUDES += ./

