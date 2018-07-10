NAME := iotx-hal
$(NAME)_SOURCES := HAL_OS_rhino.c \
				HAL_TCP_rhino.c \
				HAL_PRODUCT_rhino.c \
				HAL_UDP_rhino.c \
				HAL_Crypt_rhino.c \
				HAL_TLS_mbedtls.c \
				HAL_DTLS_mbedtls.c

$(NAME)_COMPONENTS += security/mbedtls \
    				digest_algorithm

GLOBAL_DEFINES += COAP_DTLS_SUPPORT

ifneq (,$(filter ywss4linkkit,$(COMPONENTS)))
$(NAME)_SOURCES  +=  HAL_AWSS_rhino.c
endif
