NAME := simple_mqtt

$(NAME)_SOURCES := simple_mqtt.c

$(NAME)_COMPONENTS := yloop cli connectivity.mqtt cjson fota netmgr framework.common

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

