NAME := coap

ROOT_DIR := ../../../utility/iotx-utils

$(NAME)_INCLUDES :=  \
    ./ \
    ./iot-coap-c \
    ../../../kernel/protocols/net/include/ \
    $(ROOT_DIR)/sdk-impl \
    $(ROOT_DIR)/sdk-impl/imports \
    $(ROOT_DIR)/sdk-impl/exports \
#    $(ROOT_DIR)/LITE-log \
    $(ROOT_DIR)/LITE-utils \
    $(ROOT_DIR)/digest \
#    ../../../security/mbedtls/include
    
ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
PLATFORM_COAP := linux
else 
PLATFORM_COAP := rhino
endif

$(NAME)_SOURCES := iotx_ca_cert.c iotx_coap_api.c iotx_hmac.c \
    iot-coap-c/CoAPDeserialize.c \
    iot-coap-c/CoAPExport.c \
    iot-coap-c/CoAPMessage.c \
    iot-coap-c/CoAPNetwork.c \
    iot-coap-c/CoAPSerialize.c \
    $(ROOT_DIR)/hal/$(PLATFORM_COAP)/HAL_UDP_$(PLATFORM_COAP).c \
    $(ROOT_DIR)/hal/$(PLATFORM_COAP)/HAL_OS_$(PLATFORM_COAP).c

$(NAME)_COMPONENTS += iotx-utils.LITE-utils
$(NAME)_COMPONENTS += iotx-utils.digest
$(NAME)_COMPONENTS += iotx-utils.sdk-impl
#ifeq ($(CONFIG_COAP_ONLINE), y)
#$(NAME)_DEFINES += COAP_ONLINE
#endif
ifeq ($(CONFIG_COAP_DTLS_SUPPORT), y)
$(NAME)_DEFINES += COAP_DTLS_SUPPORT
$(NAME)_COMPONENTS += iotx-utils.mbedtls-hal mbedtls
endif

# TODO: fix warnings
#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS := $(filter-out -Werror,$(CFLAGS))
endif

$(NAME)_DEFINES += DEBUG
# PKG_UPDATE  := 'git@gitlab.alibaba-inc.com:iot-middleware/iot-coap-c.git'
