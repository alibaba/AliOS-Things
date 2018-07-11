NAME := linkkit_gateway
$(NAME)_SOURCES := linkkit_sample_gateway.c \
    			light.c \
    		    linkkit_entry.c

$(NAME)_COMPONENTS += framework/protocol/iotx-rhino/iotx-sdk-c \
    				framework/protocol/iotx-rhino/hal \
					framework/netmgr \
					framework/common \
					utility/cjson \
					tools/cli \
			

GLOBAL_DEFINES += MQTT_DIRECT \
				ALIOT_DEBUG \
				IOTX_DEBUG \
				TEST_ALCS \
				CONFIG_AOS_CLI \
				USE_LPTHREAD \
				CM_SUPPORT_MULTI_THREAD_VIA_HAL \
				MBEDTLS_SSL_MAX_CONTENT_LEN=6144 
				
ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif
