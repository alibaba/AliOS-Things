NAME := libiot_wrappers

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.2
$(NAME)_SUMMARY := HAL for AliOS Things

$(NAME)_DEFINES +=  USE_YLOOP

ifeq (y,$(strip $(TLS_SAVE_TICKET)))
$(NAME)_DEFINES += os/TLS_SAVE_TICKET
endif

$(NAME)_SOURCES := os/HAL_OS_rhino.c \
                   os/HAL_TCP_rhino.c \
                   os/HAL_PRODUCT_rhino.c

$(NAME)_SOURCES-$(FS_ENABLED) += os/HAL_FS_rhino.c
$(NAME)_SOURCES-$(DEV_BIND_ENABLED) += os/HAL_AWSS_rhino.c
$(NAME)_SOURCES-$(COAP_SERVER) +=  os/HAL_UDP_rhino.c

# mandatory dependencies
$(NAME)_COMPONENTS += libiot_infra ulog activation chip_code netmgr

# optional dependencies
$(NAME)_COMPONENTS-$(SUPPORT_TLS) += libiot_certs
$(NAME)_COMPONENTS-$(USE_MBEDTLS) +=  mbedtls 
$(NAME)_COMPONENTS-$(USE_ITLS) +=  itls id2 alicrypto
ifeq (y,$(strip $(SUPPORT_TLS)))
    ifeq (y,$(strip $(USE_ITLS)))
        $(NAME)_SOURCES += os/HAL_TLS_itls.c
    else
        ifeq (y, $(strip $(MBEDTLS_CONFIG_TLS)))
        $(NAME)_DEFINES += USE_PUBLIC_MBEDTLS
        $(NAME)_SOURCES += os/HAL_TLS_mbedtls.c 
        ifeq (y,$(strip $(HAL_TLS_DNS_ENHANCE)))
            $(NAME)_SOURCES += os/dns.c
        endif  
        endif
    endif
endif

$(NAME)_COMPONENTS-$(COAP_DTLS_SUPPORT) += mbedtls libiot_certs
ifeq (y,$(strip $(COAP_COMM_ENABLED)))
ifeq (y,$(strip $(COAP_DTLS_SUPPORT)))
$(info COAP_SUPPORT_DTLS = y, so compile DTLS)
$(NAME)_SOURCES += os/HAL_DTLS_mbedtls.c
$(NAME)_DEFINES += USE_PUBLIC_MBEDTLS
endif
$(NAME)_SOURCES += os/HAL_UDP_rhino.c
endif
