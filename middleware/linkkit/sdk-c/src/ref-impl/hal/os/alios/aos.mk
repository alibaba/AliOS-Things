NAME := iotx-hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := HAL for AliOS Things

$(NAME)_SOURCES := HAL_OS_rhino.c \
                   HAL_TCP_rhino.c \
                   HAL_PRODUCT_rhino.c \
                   HAL_UDP_rhino.c \
                   HAL_AWSS_rhino.c

$(NAME)_SOURCES-$(FS_ENABLED) += HAL_FS_rhino.c

$(NAME)_COMPONENTS += ulog activation chip_code netmgr

GLOBAL_DEFINES +=RHINO_CONFIG_TIMER_TASK_STACK_SIZE=1024

ifeq (y,$(strip $(SUPPORT_ITLS)))
$(info FEATURE_SUPPORT_ITLS = y, so using iTLS)
$(NAME)_SOURCES += HAL_TLS_itls.c    HAL_Crypt_rhino.c 
$(NAME)_COMPONENTS += itls
else
ifeq (y,$(strip $(SUPPORT_TLS)))
$(info FEATURE_SUPPORT_ITLS != y, so using normal TLS)
$(NAME)_SOURCES += HAL_TLS_mbedtls.c  HAL_Crypt_rhino.c 
$(NAME)_COMPONENTS += imbedtls

ifeq (y,$(strip $(COAP_SUPPORT_DTLS)))
$(info FEATURE_COAP_SUPPORT_DTLS = y, so using DTLS)
$(NAME)_SOURCES += HAL_DTLS_mbedtls.c
GLOBAL_DEFINES += COAP_DTLS_SUPPORT
endif
endif
endif
