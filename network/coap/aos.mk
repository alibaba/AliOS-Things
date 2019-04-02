NAME := libcoap

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := libcoap component

$(NAME)_COMPONENTS := imbedtls activation chip_code libiot_sdk_impl libiot_system libiot_utils iotx-hal

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
                   $(LIBCOAP_SRC_PATH)/coap_mbeddtls.c \
                   $(LIBCOAP_SRC_PATH)/mem.c  \
                   ./iotx_coap_api.c

GLOBAL_INCLUDES += ./libcoap-4.2.0/include/coap2/

GLOBAL_INCLUDES += $(EXT_SRC_PATH)/include/                  \
                   $(EXT_SRC_PATH)/include/imports/          \
                   $(EXT_SRC_PATH)/include/exports/          \
                   $(EXT_SRC_PATH)/src/protocol/coap/        \
                   $(EXT_SRC_PATH)/src/infra/utils/digest/   \
                   $(EXT_SRC_PATH)/src/infra/utils/misc/     \
                   $(EXT_SRC_PATH)/src/infra/utils/          \
                   $(EXT_SRC_PATH)/src/infra/log/


GLOBAL_DEFINES += NDEBUG COAP_COMM_ENABLED BUILD_AOS UTILS_MD5 UTILS_STRING UTILS_JSON_PARSER UTILS_SHA256 MQTT_DIRECT
