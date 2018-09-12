NAME := uOTA

$(NAME)_TYPE := framework

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall
endif

$(NAME)_COMPONENTS += middleware.uagent.uota.src.recovery

$(NAME)_SOURCES += \
    hal/ota_hal_plat.c \
    src/otalib/ota.c \
    src/service/ota_service.c \
    src/service/ota_version.c \
    src/transport/ota_transport_mqtt.c \
    src/transport/ota_transport_coap.c \
    src/download/ota_manifest.c \
    src/download/ota_download_http.c \
    src/download/ota_download_coap.c \
    src/verify/ota_util.c \
    src/verify/ota_verify.c \
    src/verify/ota_rsa_verify.c \
    src/verify/hash/ota_hash.c \
    src/verify/hash/ota_md5.c \
    src/verify/hash/ota_sha256.c \
    src/verify/rsa/ota_bignum.c \
    src/verify/rsa/ota_rsa.c \
    src/verify/rsa/ota_hash_rsa.c \
    src/utility/base64/ota_base64.c \
    src/utility/hexstr2hexbuf/ota_hexstr2hexbuf.c \
    hal/ota_hal_os.c

$(NAME)_INCLUDES += \
    src/service \
    src/transport \
    src/download  \
    src/verify  \
    src/verify/rsa  \
    src/verify/hash  \
    src/utility/cjson \
    src/utility/crc \
    src/utility/base64 \
    src/utility/hexstr2hexbuf \
    hal \
    inc

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ESP8266)
GLOBAL_DEFINES += IS_ESP8266
GLOBAL_DEFINES += OTA_MD5_CHECK
GLOBAL_DEFINES += OTA_SUPPORT_DIFF
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3060)
GLOBAL_DEFINES += OTA_MD5_CHECK
GLOBAL_DEFINES += OTA_SUPPORT_DIFF
endif

ifeq ($(MD5_CHECK), 1)
GLOBAL_DEFINES += OTA_MD5_CHECK
endif
ifeq ($(HTTPS_DL),1)
GLOBAL_DEFINES += HTTPS_DOWNLOAD
endif
ifeq ($(ITLS_DL),1)
GLOBAL_DEFINES += ITLS_DOWNLOAD
endif

GLOBAL_DEFINES += USE_LPTHREAD
GLOBAL_DEFINES += OTA_ALIOS
GLOBAL_DEFINES += OTA_WITH_LINKKIT
GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
#GLOBAL_DEFINES += OTA_BREAKPOINT_SUPPORT
#GLOBAL_DEFINES += OTA_MULTI_BINS
