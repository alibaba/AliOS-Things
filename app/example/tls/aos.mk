NAME := tls_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := TLS client example for imbedtls
DTLS := no

$(NAME)_SOURCES     := tls_client.c

ifeq ($(DTLS), yes)
$(NAME)_DEFINES     += DTLS_ENABLED
$(NAME)_SOURCES     += dtls_client.c
endif

$(NAME)_COMPONENTS  := imbedtls cli alicrypto netmgr

GLOBAL_INCLUDES += ./
