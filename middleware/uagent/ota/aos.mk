NAME := ota

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := An over-the-air update is the wireless delivery of new software. 

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_INCLUDES += include hal ota_core/crypto

$(NAME)_SOURCES := ota_core/ota_service.c \
                   ota_core/download/ota_download_http.c   \
                   ota_core/transport/ota_transport_mqtt.c \
                   ota_core/crypto/ota_crypto_hash.c       \
                   ota_core/crypto/ota_crypto_sign.c       \
                   hal/ota_hal_plat.c                      \
                   hal/ota_hal_common.c                    \
                   hal/ota_plat_ctrl.c

$(NAME)_COMPONENTS += linkkit_sdk_c cjson http

ifeq ($(FEATURE_SUPPORT_ITLS),y)
   $(NAME)_COMPONENTS += itls
   ifeq ($(COMPILER),armcc)
   GLOBAL_CFLAGS += -DOTA_CONFIG_ITLS -DCONFIG_HTTP_SECURE_ITLS=1 -DOTA_SIGNAL_CHANNEL=1 -DCONFIG_HTTP_SECURE=1
   else
   GLOBAL_DEFINES += OTA_CONFIG_ITLS CONFIG_HTTP_SECURE_ITLS=1 OTA_SIGNAL_CHANNEL=1 CONFIG_HTTP_SECURE=1
   endif
else
   $(NAME)_COMPONENTS += mbedtls
   ifeq ($(COMPILER),armcc)
       ifeq ($(HTTPS_DL),1)
       GLOBAL_CFLAGS += -DOTA_CONFIG_SECURE_DL_MODE -DMBEDTLS_CONFIG_TLS_MAX_CONTENT_LEN=8192
       endif
       GLOBAL_CFLAGS += -DOTA_SIGNAL_CHANNEL=1 -DCONFIG_HTTP_SECURE=1
   else
       ifeq ($(HTTPS_DL),1)
       GLOBAL_DEFINES += OTA_CONFIG_SECURE_DL_MODE MBEDTLS_CONFIG_TLS_MAX_CONTENT_LEN=8192
       endif
       GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1 CONFIG_HTTP_SECURE=1
   endif
endif
