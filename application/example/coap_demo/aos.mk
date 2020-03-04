NAME := coap_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 2.3.2
$(NAME)_SUMMARY := coap example

$(NAME)_SOURCES += app_entry.c    \
                   coap_example.c \
                   maintask.c

$(NAME)_COMPONENTS += libcoap netmgr cli

$(NAME)_INCLUDES += ./

