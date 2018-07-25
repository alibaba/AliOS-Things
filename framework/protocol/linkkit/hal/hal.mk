NAME := iotx-hal
$(NAME)_SOURCES := HAL_OS_rhino.c \
                   HAL_TCP_rhino.c \
                   HAL_PRODUCT_rhino.c \
                   HAL_UDP_rhino.c \
                   HAL_Crypt_rhino.c \
                   HAL_AWSS_rhino.c

$(NAME)_COMPONENTS += \
                      digest_algorithm \
                      activation \
                      chip_code

GLOBAL_DEFINES += COAP_DTLS_SUPPORT

ifneq (,$(filter ywss4linkkit,$(COMPONENTS)))
$(NAME)_SOURCES  +=  HAL_AWSS_rhino.c
endif

IOTX_SETTING_PATH := $(SOURCE_ROOT)example/$(APP_FULL)
SWITCH_ITLS := $(shell set -x && \
    [ -f $(IOTX_SETTING_PATH)/make.settings ] && \
    grep '^FEATURE_SUPPORT_ITLS' $(IOTX_SETTING_PATH)/make.settings | \
        awk '{ print $$NF }')

ifeq (y,$(strip $(SWITCH_ITLS)))
$(warning SWITCH_ITLS from [$(IOTX_SETTING_PATH)/make.settings] is $(SWITCH_ITLS), so using iTLS)
$(NAME)_SOURCES += HAL_TLS_itls.c
$(NAME)_COMPONENTS += security/itls
else
$(warning SWITCH_ITLS from [$(IOTX_SETTING_PATH)/make.settings] is $(SWITCH_ITLS), so using normal TLS)
$(NAME)_SOURCES += HAL_TLS_mbedtls.c HAL_DTLS_mbedtls.c
$(NAME)_COMPONENTS += security/mbedtls
endif
