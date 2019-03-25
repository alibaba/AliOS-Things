NAME := iotx-hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := HAL for AliOS Things

$(NAME)_SOURCES := HAL_OS_rhino.c \
                   HAL_TCP_rhino.c \
                   HAL_PRODUCT_rhino.c \
                   HAL_UDP_rhino.c 

$(NAME)_DEFINES +=  USE_YLOOP

$(NAME)_SOURCES-$(FS_ENABLED) += HAL_FS_rhino.c

$(NAME)_COMPONENTS += ulog activation chip_code netmgr

ifeq (y,$(strip $(DEV_BIND_ENABLED)))
$(NAME)_SOURCES += HAL_AWSS_rhino.c

endif

ifeq (y,$(strip $(HAL_CRYPTO)))
$(NAME)_SOURCES += HAL_Crypt_rhino.c 
ifeq (y,$(strip $(SUPPORT_ITLS)))
$(NAME)_COMPONENTS +=  itls
else
ifeq (y,$(strip $(SUPPORT_TLS)))
$(NAME)_DEFINES += USE_PUBLIC_MBEDTLS
$(NAME)_COMPONENTS +=  mbedtls
else 
$(NAME)_DEFINES += USE_PUBLIC_MBEDTLS
$(NAME)_COMPONENTS +=  mbedtls
endif
endif
endif

ifeq (y,$(strip $(SUPPORT_ITLS)))
$(info SUPPORT_ITLS = y, so using iTLS)
$(NAME)_SOURCES += HAL_TLS_itls.c    
$(NAME)_SOURCES += HAL_Crypt_rhino.c 
$(NAME)_COMPONENTS += itls
else
ifeq (y,$(strip $(SUPPORT_TLS)))
$(info SUPPORT_TLS = y, so using normal TLS)
$(NAME)_SOURCES += HAL_TLS_mbedtls.c 
$(NAME)_SOURCES += HAL_Crypt_rhino.c 
$(NAME)_COMPONENTS += mbedtls


ifeq (y,$(strip $(COAP_SUPPORT_DTLS)))
$(info COAP_SUPPORT_DTLS = y, so compile DTLS)
$(NAME)_SOURCES += HAL_DTLS_mbedtls.c
$(NAME)_COMPONENTS += mbedtls
GLOBAL_DEFINES += COAP_DTLS_SUPPORT
endif
endif
endif

ifeq (y,$(strip $(TLS_SAVE_TICKET)))
$(NAME)_DEFINES += TLS_SAVE_TICKET
endif
