NAME := coapapp
$(NAME)_SOURCES := app_entry.c \
    			coap-example.c

$(NAME)_COMPONENTS += framework/protocol/linkkit/sdk \
    				framework/protocol/linkkit/hal \
					framework/netmgr \
					framework/common \
					utility/cjson \
					tools/cli 

GLOBAL_DEFINES += CONFIG_AOS_CLI COAP_DTLS_SUPPORT

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif
