NAME := coapapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := coap cloud example
$(NAME)_SOURCES := app_entry.c \
                   coap_example.c

$(NAME)_COMPONENTS += feature.linkkit-coap netmgr cjson cli

GLOBAL_DEFINES += CONFIG_AOS_CLI

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

ifeq ($(loop),1)
$(NAME)_DEFINES      += TEST_LOOP
endif

GLOBAL_INCLUDES += ./

