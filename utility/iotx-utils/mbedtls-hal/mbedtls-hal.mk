NAME := libmbedtls-hal

$(NAME)_INCLUDES :=  \
    ../../../security/mbedtls/include \
    ../sdk-impl \
    ../sdk-impl/imports
    
ifeq ($(CONFIG_COAP_DTLS_SUPPORT), y)
$(NAME)_DEFINES += COAP_DTLS_SUPPORT
endif

$(NAME)_DEFINES += FORCE_SSL_VERIFY

$(NAME)_SOURCES := \
    HAL_DTLS_mbedtls.c \
    HAL_TLS_mbedtls.c

$(NAME)_COMPONENTS  := mbedtls alicrypto


$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))

$(NAME)_DEFINES += DEBUG
