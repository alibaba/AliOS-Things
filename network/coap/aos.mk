NAME := libcoap

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := libcoap component

LIBCOAP_SRC_PATH := ./src/libcoap-4.2.0/

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
                   ./src/iotx_coap_api.c

ifeq (y,$(AOS_COMP_LWM2M))
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/wakaa_lwm2m_adapter.c
endif

ifeq (y,$(COAP_WITH_DTLS))
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/coap_mbedtls.c
else
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/coap_notls.c
endif

GLOBAL_INCLUDES += ./include/coap2/     \
                   ./include/

ifeq (y,$(COAP_WITH_ALI_AUTH))
$(NAME)_COMPONENTS += mbedtls activation chip_code libiot_infra libiot_wrappers
GLOBAL_DEFINES += BUILD_AOS INFRA_MD5 WITH_LIBCOAP_DEBUG
else
GLOBAL_DEFINES += NDEBUG COAP_WITH_NOAUTH WITH_LIBCOAP_DEBUG
endif

include $($(NAME)_LOCATION)/wrappers/coap_wrapper.mk

