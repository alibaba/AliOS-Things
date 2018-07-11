NAME := linkkitapp
$(NAME)_SOURCES := linkkit_sample_single.c \
    		    linkkit_entry.c

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
				CONFIG_DM_DEVTYPE_SINGLE \
				TEST_ALCS \
				CONFIG_AOS_CLI \
				NDEBUG 
				
ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif
