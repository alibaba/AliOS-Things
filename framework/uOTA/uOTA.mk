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

ifeq ($(HTTPS_DL),1)
GLOBAL_DEFINES    += HTTPS_DOWNLOAD
endif
GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1
GLOBAL_DEFINES += USE_LPTHREAD
GLOBAL_DEFINES += OTA_BREAKPOINT_SUPPORT
#GLOBAL_DEFINES += OTA_MULTI_BINS
