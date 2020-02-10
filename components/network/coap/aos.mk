NAME := libcoap

# component information
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := libcoap component

# source files and the folder of internal include files
$(NAME)_INCLUDES += ./include/coap2/     \
                    ./include/

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
                   ./src/iotx_coap_api.c      \
                   ./wrappers/coap_infra_wrapper.c

ifeq (y,$(AOS_COMP_LWM2M))
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/wakaa_lwm2m_adapter.c
endif

ifeq (y,$(COAP_WITH_DTLS))
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/coap_mbedtls.c
else
$(NAME)_SOURCES += $(LIBCOAP_SRC_PATH)/coap_notls.c
endif


# the folder of API files
GLOBAL_INCLUDES += ../../../include/network/coap

# mandatory dependencies

# optional dependencies
$(NAME)_COMPONENTS-$(COAP_WITH_ALI_AUTH) += mbedtls activation chip_code libiot_infra libiot_wrappers
