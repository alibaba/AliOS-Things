NAME := coapapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := coap cloud example
$(NAME)_SOURCES := app_entry.c \
                   coap_example.c

$(NAME)_COMPONENTS += linkkit_sdk_c netmgr cjson cli

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif


GLOBAL_INCLUDES += ./

