NAME := itls_client

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 2.0.0
$(NAME)_SUMMARY := itls sample app

$(NAME)_SOURCES := app_entry.c
$(NAME)_SOURCES += itls_client.c
$(NAME)_SOURCES += idtls_client.c

$(NAME)_COMPONENTS  := itls id2 alicrypto netmgr cli

#$(NAME)_CFLAGS += -DON_DAILY

ifeq ($(HOST_ARCH), linux)
$(NAME)_CFLAGS += -DCONFIG_SSL_DTLS
endif

$(NAME)_INCLUDES += ./
