NAME := coapapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := coap example
$(NAME)_SOURCES := app_entry.c

ifneq ($(AOS_COMP_SAL),y)
$(NAME)_SOURCES += coap_example.c
$(NAME)_COMPONENTS += libcoap netmgr cjson cli ulog
endif

$(NAME)_INCLUDES += ./

