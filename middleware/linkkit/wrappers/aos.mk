NAME := libiot_wrappers

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 3.0.1
$(NAME)_SUMMARY := HAL for AliOS Things

$(NAME)_COMPONENTS += libiot_infra

$(NAME)_SOURCES := os/HAL_OS_rhino.c \
                   os/HAL_TCP_rhino.c \
                   os/HAL_PRODUCT_rhino.c \

$(NAME)_DEFINES +=  USE_YLOOP

$(NAME)_SOURCES-$(FS_ENABLED) += os/HAL_FS_rhino.c

$(NAME)_COMPONENTS += ulog activation chip_code netmgr

GLOBAL_DEFINES += BUILD_AOS
$(NAME)_DEFINES += USE_YLOOP

ifeq (y,$(strip $(DEV_BIND_ENABLED)))
$(NAME)_SOURCES += os/HAL_AWSS_rhino.c
endif

ifeq (y,$(strip $(COAP_SERVER)))
$(NAME)_SOURCES += os/HAL_UDP_rhino.c
endif

ifeq (y,$(strip $(SUPPORT_ITLS)))
$(NAME)_COMPONENTS +=  itls 
$(NAME)_SOURCES += os/HAL_TLS_itls.c
else
ifeq (y,$(strip $(SUPPORT_TLS)))
$(NAME)_DEFINES += USE_PUBLIC_MBEDTLS
$(NAME)_COMPONENTS +=  mbedtls libiot_certs
$(NAME)_SOURCES += os/HAL_TLS_mbedtls.c 
endif
endif


ifeq (y,$(strip $(COAP_COMM_ENABLED)))
ifeq (y,$(strip $(COAP_DTLS_SUPPORT)))
$(info COAP_SUPPORT_DTLS = y, so compile DTLS)
$(NAME)_SOURCES += os/HAL_DTLS_mbedtls.c
$(NAME)_COMPONENTS += mbedtls libiot_certs
$(NAME)_DEFINES += USE_PUBLIC_MBEDTLS
endif
$(NAME)_SOURCES += os/HAL_UDP_rhino.c
endif 

ifeq (y,$(strip $(TLS_SAVE_TICKET)))
$(NAME)_DEFINES += os/TLS_SAVE_TICKET
endif

GLOBAL_INCLUDES += . 
