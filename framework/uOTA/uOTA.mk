NAME := uOTA

$(NAME)_TYPE := framework

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall
endif

$(NAME)_SOURCES += \
    src/service/ota_service.c \
    src/service/ota_version.c \
    src/transport/ota_transport_mqtt.c \
    src/transport/ota_transport_coap.c \
    src/download/ota_manifest.c \
    src/download/ota_download_http.c \
    src/download/ota_download_coap.c \
    src/download/ota_socket.c \
    src/verify/ota_util.c \
    src/verify/ota_verify.c \
    hal/ota_hal_os.c

$(NAME)_INCLUDES += \
    src/service \
    src/transport \
    src/download  \
    src/verify  \
    src/utility/cjson \
    src/utility/crc \
    hal \
    inc

IOTX_SETTING_PATH := $(SOURCE_ROOT)example/$(APP_FULL)
SWITCH_ITLS := $(shell set -x && \
    [ -f $(IOTX_SETTING_PATH)/make.settings ] && \
    grep '^FEATURE_SUPPORT_ITLS' $(IOTX_SETTING_PATH)/make.settings | \
        awk '{ print $$NF }')

ifeq (y,$(strip $(SWITCH_ITLS)))
$(warning SWITCH_ITLS from [$(IOTX_SETTING_PATH)/make.settings] is $(SWITCH_ITLS), so using iTLS)
GLOBAL_DEFINES += ITLS_DOWNLOAD
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += IS_ESP8266
endif

ifeq ($(MD5_CHECK),1)
GLOBAL_DEFINES += SUPPORT_MD5_CHECK
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3060)
GLOBAL_DEFINES += SUPPORT_MD5_CHECK
endif

ifeq ($(HTTPS_DL),1)
GLOBAL_DEFINES    += HTTPS_DOWNLOAD
endif
GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
GLOBAL_DEFINES += USE_LPTHREAD
GLOBAL_DEFINES += OTA_BREAKPOINT_SUPPORT
#GLOBAL_DEFINES += OTA_MULTI_BINS
