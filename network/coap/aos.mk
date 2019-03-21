NAME := libcoap

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := libcoap component

$(NAME)_COMPONENTS := imbedtls activation chip_code

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
                   ./iotx_coap_api.c        \
                   ./coap_debug.c           \
                   ./coap_mbeddtls.c        \
                   ./mem.c

GLOBAL_INCLUDES += ./libcoap-4.2.0/include/coap2/

$(NAME)_SOURCES += $(EXT_SRC_PATH)/src/infra/utils/digest/*.c \
                   $(EXT_SRC_PATH)/src/infra/utils/misc/*.c   \
                   $(EXT_SRC_PATH)/src/infra/log/*.c          \
                   $(EXT_SRC_PATH)/src/infra/system/guider.c  \
                   $(EXT_SRC_PATH)/src/infra/system/ca.c      \
                   $(EXT_SRC_PATH)/src/ref-impl/hal/os/alios/HAL_OS_rhino.c \
                   $(EXT_SRC_PATH)/src/ref-impl/hal/os/alios/HAL_PRODUCT_rhino.c \
                   $(EXT_SRC_PATH)/src/sdk-impl/sdk-impl.c

GLOBAL_INCLUDES += $(EXT_SRC_PATH)/include/                  \
                   $(EXT_SRC_PATH)/include/imports/          \
                   $(EXT_SRC_PATH)/include/exports/          \
                   $(EXT_SRC_PATH)/src/protocol/coap/        \
                   $(EXT_SRC_PATH)/src/infra/utils/digest/   \
                   $(EXT_SRC_PATH)/src/infra/utils/misc/     \
                   $(EXT_SRC_PATH)/src/infra/utils/          \
                   $(EXT_SRC_PATH)/src/infra/log/            \
                   $(EXT_SRC_PATH)/src/infra/system/


GLOBAL_DEFINES += COAP_COMM_ENABLED UTILS_MD5 UTILS_STRING UTILS_JSON_PARSER UTILS_SHA256 MQTT_DIRECT
