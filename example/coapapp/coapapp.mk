NAME := coapapp
$(NAME)_SOURCES := app_entry.c \
    			coap-example.c

$(NAME)_COMPONENTS += framework/protocol/iotx-rhino/iotx-sdk-c \
    				framework/protocol/iotx-rhino/hal \
					framework/netmgr \
					framework/common \
					utility/cjson \
					tools/cli

GLOBAL_DEFINES += MQTT_DIRECT \
				ALIOT_DEBUG \
				IOTX_DEBUG \
				USE_LPTHREAD \
				CONFIG_AOS_CLI

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif
