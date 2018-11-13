NAME := itls_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES     := itls_client.c

$(NAME)_COMPONENTS  := itls id2 alicrypto netmgr cli

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += network.lwip
no_with_lwip := 0
endif

GLOBAL_INCLUDES += ./