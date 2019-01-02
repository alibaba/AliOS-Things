NAME := tls_client

DTLS := no

$(NAME)_SOURCES     := tls_client.c

ifeq ($(DTLS), yes)
$(NAME)_DEFINES     += DTLS_ENABLED
$(NAME)_SOURCES     += dtls_client.c
endif

$(NAME)_COMPONENTS  := mbedtls cli alicrypto netmgr

GLOBAL_INCLUDES += ./