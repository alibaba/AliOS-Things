NAME := libcoap

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := libcoap component

$(NAME)_COMPONENTS := imbedtls

LIBCOAP_SRC_PATH := ./libcoap-4.2.0/src

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
                   ./mem.c                  \
                   ./external/utils/digest/*.c \
                   ./external/utils/misc/*.c   \
                   ./external/log/*.c          \
                   ./external/hal_impl.c

GLOBAL_INCLUDES += ./libcoap-4.2.0/include/coap2/  \
                   ./external/include/                \
                   ./external/utils/digest/           \
                   ./external/utils/misc/             \
                   ./external/utils/                  \
                   ./external/log/


GLOBAL_DEFINES += COAP_COMM_ENABLED UTILS_MD5 UTILS_STRING UTILS_JSON_PARSER
