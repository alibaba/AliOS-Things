NAME := coapapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := app_entry.c \
                   coap_example.c

$(NAME)_COMPONENTS += feature.linkkit-coap \
                      network/netmgr \
                      middleware/common \
                      utility/cjson \
                      tools/cli

GLOBAL_DEFINES += CONFIG_AOS_CLI

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

ifeq ($(loop),1)
$(NAME)_DEFINES      += TEST_LOOP
endif

GLOBAL_INCLUDES += ./

