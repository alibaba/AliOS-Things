#NAME := iotkit-system

$(NAME)_SOURCES  +=  hal-impl/rhino/HAL_OS_rhino.c
$(NAME)_SOURCES  +=  hal-impl/rhino/HAL_TCP_rhino.c
$(NAME)_SOURCES  +=  hal-impl/rhino/HAL_PRODUCT_rhino.c
$(NAME)_SOURCES  +=  hal-impl/rhino/HAL_UDP_rhino.c
ifneq (,$(filter ywss4linkkit,$(COMPONENTS)))
$(NAME)_SOURCES  +=  hal-impl/rhino/HAL_AWSS_rhino.c
endif
$(NAME)_SOURCES  += hal-impl/tls/mbedtls/HAL_DTLS_mbedtls.c
$(NAME)_SOURCES  += hal-impl/tls/mbedtls/HAL_TLS_mbedtls.c
$(NAME)_INCLUDES += ./
GLOBAL_INCLUDES += hal-impl 
$(NAME)_COMPONENTS += mbedtls
