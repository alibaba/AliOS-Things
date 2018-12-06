NAME := athostapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := An example for receiving and processing AT command sets.
$(NAME)_SOURCES := athostapp.c
$(NAME)_COMPONENTS := athost netmgr cli yloop

no_atparser ?= 0
ifneq (1,$(no_atparser))
$(NAME)_COMPONENTS += atparser
endif

no_mqttreport ?= 0
ifeq (1,$(no_mqttreport))
GLOBAL_DEFINES += ATHOST_MQTT_REPORT_DISBALED
endif
GLOBAL_DEFINES += DEBUG

$(NAME)_INCLUDES := ./

GLOBAL_INCLUDES += ./

HTTP2APP = 1
