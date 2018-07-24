NAME := itls_client

$(NAME)_SOURCES     := itls_client.c

$(NAME)_COMPONENTS  := itls id2 alicrypto netmgr cli

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

