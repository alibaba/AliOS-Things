NAME := itls_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.1.0
$(NAME)_SUMMARY := itls sample app

$(NAME)_SOURCES     := itls_client.c

$(NAME)_COMPONENTS  := itls id2 alicrypto netmgr cli

GLOBAL_CFLAGS += -DON_DAILY

GLOBAL_INCLUDES += ./
