NAME := athostapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := An example for receiving and processing AT command sets.
$(NAME)_SOURCES := athostapp.c
$(NAME)_COMPONENTS := athost netmgr cli yloop

ATHOSTAPP_CONFIG_ATPARSER ?= y
ATHOSTAPP_CONFIG_MQTTREPORT ?= y

ifeq ($(ATHOSTAPP_CONFIG_ATPARSER),y)
$(NAME)_COMPONENTS += atparser
endif

ifeq ($(ATHOSTAPP_CONFIG_MQTTREPORT),n)
GLOBAL_DEFINES += ATHOST_MQTT_REPORT_DISBALED
endif
GLOBAL_DEFINES += DEBUG

$(NAME)_INCLUDES := ./