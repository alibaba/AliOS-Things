NAME := athostapp

$(NAME)_SOURCES := athostapp.c
$(NAME)_COMPONENTS := network/sal/athost \
                      network/netmgr \
                      tools/cli \
                      yloop

no_atparser ?= 0
ifneq (1,$(no_atparser))
$(NAME)_COMPONENTS += network/sal/atparser
endif

no_mqttreport ?= 0
ifeq (1,$(no_mqttreport))
GLOBAL_DEFINES += ATHOST_MQTT_REPORT_DISBALED
endif
GLOBAL_DEFINES += DEBUG

$(NAME)_INCLUDES := ./

GLOBAL_INCLUDES += ./
