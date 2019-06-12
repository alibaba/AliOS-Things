NAME := libcoap

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := libcoap component

LIBCOAP_SRC_PATH := ./libcoap-4.2.0/src

EXT_SRC_PATH := ../../middleware/linkkit/sdk-c

$(NAME)_SOURCES := $(LIBCOAP_SRC_PATH)/option.c \
                   $(LIBCOAP_SRC_PATH)/resource.c \
                   $(LIBCOAP_SRC_PATH)/pdu.c \
                   $(LIBCOAP_SRC_PATH)/encode.c \
                   $(LIBCOAP_SRC_PATH)/subscribe.c \
                   $(LIBCOAP_SRC_PATH)/coap_io.c \
                   $(LIBCOAP_SRC_PATH)/block.c \
                   $(LIBCOAP_SRC_PATH)/uri.c \
                   $(LIBCOAP_SRC_PATH)/str.c \
                   $(LIBCOAP_SRC_PATH)/coap_session.c \
                   $(LIBCOAP_SRC_PATH)/coap_hashkey.c \
                   $(LIBCOAP_SRC_PATH)/coap_time.c \
                   $(LIBCOAP_SRC_PATH)/net.c \
                   $(LIBCOAP_SRC_PATH)/address.c \
                   $(LIBCOAP_SRC_PATH)/coap_debug.c  \
                   $(LIBCOAP_SRC_PATH)/mem.c  \
                   ./iotx_coap_api.c

ifeq (y,$(AOS_COMP_LWM2M))
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/wakaa_lwm2m_adapter.c
endif

ifeq (y,$(COAP_WITH_DTLS))
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/coap_mbedtls.c
else
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/coap_notls.c
endif

GLOBAL_INCLUDES += ./libcoap-4.2.0/include/coap2/

GLOBAL_INCLUDES += $(EXT_SRC_PATH)/include/                  \
                   $(EXT_SRC_PATH)/include/imports/          \
                   $(EXT_SRC_PATH)/include/exports/          \
                   $(EXT_SRC_PATH)/src/protocol/coap/        \
                   $(EXT_SRC_PATH)/src/infra/utils/digest/   \
                   $(EXT_SRC_PATH)/src/infra/utils/misc/     \
                   $(EXT_SRC_PATH)/src/infra/utils/          \
                   $(EXT_SRC_PATH)/src/infra/log/

#ifeq (y,$(COAP_WITH_ALI_AUTH))
$(NAME)_COMPONENTS += mbedtls activation chip_code libiot_sdk_impl libiot_system libiot_utils iotx-hal
GLOBAL_DEFINES += NDEBUG COAP_COMM_ENABLED BUILD_AOS UTILS_MD5 UTILS_STRING UTILS_JSON_PARSER UTILS_SHA256 MQTT_DIRECT USE_EXTERNAL_MEBDTLS WITH_LIBCOAP_DEBUG
#else
#GLOBAL_DEFINES += NDEBUG COAP_COMM_ENABLED COAP_WITH_NOAUTH WITH_LIBCOAP_DEBUG
#endif
