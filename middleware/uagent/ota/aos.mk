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

$(NAME)_COMPONENTS += linkkit_sdk_c cjson

$(NAME)_SOURCES := ota_core/ota_service.c \
                   ota_core/download/ota_download_http.c \
                   ota_core/transport/ota_transport_mqtt.c \
                   ota_core/verify/ota_hash.c \
                   ota_core/verify/ota_sign.c

$(NAME)_SOURCES += hal/ota_hal_module.c \
                   hal/ota_hal_os.c

ifneq (,$(filter mcu_linuximpl,$(HOST_MCU_FAMILY)))
$(NAME)_SOURCES += hal/ota_hal_host.c
else
ifneq (,$(filter mcu_esp32,$(HOST_MCU_FAMILY)))
$(NAME)_SOURCES += hal/ota_hal_esp32.c
else
ifneq (,$(filter mcu_bk7231u,$(HOST_MCU_FAMILY)))
$(NAME)_SOURCES += hal/ota_hal_bk7231u.c
else
$(NAME)_SOURCES += hal/ota_hal_plat.c
endif
endif
endif

ifeq ($(FEATURE_SUPPORT_ITLS),y)
GLOBAL_DEFINES += OTA_CONFIG_ITLS
$(NAME)_COMPONENTS += itls
else
$(NAME)_COMPONENTS += mbedtls
endif

GLOBAL_INCLUDES += include hal ota_core/verify 2nd_boot
GLOBAL_DEFINES += OTA_CONFIG_MQTT OTA_CONFIG_RSA
ifeq ($(HTTPS_DL),1)
GLOBAL_DEFINES +=OTA_CONFIG_TLS
endif
GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
