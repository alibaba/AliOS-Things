NAME := coapapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 2.3.2
$(NAME)_SUMMARY := coap example
$(NAME)_SOURCES := app_entry.c

$(NAME)_SOURCES += coap_example.c
$(NAME)_COMPONENTS += libcoap netmgr cli

$(NAME)_INCLUDES += ./

