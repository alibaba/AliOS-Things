NAME := itls_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.1.0
$(NAME)_SUMMARY := itls sample app

$(NAME)_SOURCES     := itls_client.c

$(NAME)_COMPONENTS  := itls id2 alicrypto netmgr cli

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += lwip
no_with_lwip := 0
endif

GLOBAL_CFLAGS += -DON_DAILY

GLOBAL_INCLUDES += ./
