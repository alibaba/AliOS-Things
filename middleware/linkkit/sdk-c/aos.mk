NAME := linkkit_sdk_c

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY := IoT Connectivity Devkit Running on Embedded Devices for Alibaba Cloud

GLOBAL_INCLUDES += \
    include/exports \
    include/imports \
    include

GLOBAL_CFLAGS  += -D_IS_LINKKIT_
GLOBAL_CFLAGS  += -DFORCE_SSL_VERIFY
GLOBAL_CFLAGS  += \
    -DCOAP_OBSERVE_SERVER_SUPPORTED \
    -DCOAP_OBSERVE_CLIENT_SUPPORTED \
    -DCOAP_SERV_MULTITHREAD \
    -DCOAP_USE_PLATFORM_MEMORY -DCOAP_USE_PLATFORM_LOG

#from src/board/config.rhino.make
GLOBAL_CFLAGS  +=

GLOBAL_DEFINES  += BUILD_AOS

GLOBAL_DEFINES   += \
    WITH_MQTT_ZIP_TOPIC=1 \
    WITH_MQTT_SUB_SHORTCUT=1 \
    WITH_MQTT_DYN_BUF=1

ROOT_DIR := ../../../
GLOBAL_INCLUDES += $(ROOT_DIR)middleware/linkkit/sdk-c/src/sdk-impl \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/utils/digest \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/utils \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/utils/misc \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/log \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/system \
$(ROOT_DIR)middleware/linkkit/sdk-c/include/exports \
$(ROOT_DIR)middleware/linkkit/sdk-c/include/imports \
$(ROOT_DIR)middleware/linkkit/sdk-c/include \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/alcs \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/coap/server \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/linkkit/dev_reset \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/dev_bind \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/dev_bind/utility \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/awss \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/linkkit/dm \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/linkkit/cm \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/mdal/mal \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/ota \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/ota/impl \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/utils/misc  \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/sdk-impl \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/mqtt \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/mqtt/client

#from src/tools/default_settings.mk
GLOBAL_DEFINES  += \
    COAP_SERV_MULTITHREAD \

CONFIG_LIB_EXPORT ?= static

#####################################################################
# Configs for component middleware.linkkit.sdk-c
#

$(NAME)_COMPONENTS-y := iotx-hal
$(NAME)_COMPONENTS-$(MAL_ENABLED) += libiot_mal
$(NAME)_COMPONENTS-$(DEVICE_MODEL_ENABLED) += libiot_cm libiot_dm libdev_reset
$(NAME)_COMPONENTS-$(DEV_BIND_ENABLED) += libdev_bind libiot_coap_local libiot_mqtt
$(NAME)_COMPONENTS-$(WIFI_PROVISION_ENABLED) += libawss libiot_coap_local
$(NAME)_COMPONENTS-$(MQTT_COMM_ENABLED) += libiot_mqtt
$(NAME)_COMPONENTS-$(COAP_COMM_ENABLED) += libiot_coap_cloud
$(NAME)_COMPONENTS-$(HTTP_COMM_ENABLED) += libiot_http
$(NAME)_COMPONENTS-$(ALCS_ENABLED) += libiot_alcs libiot_coap_local
$(NAME)_COMPONENTS-$(HTTP2_COMM_ENABLED) += libiot_http2 libiot_http2_stream

#####################################################################
# Process dependencies of configurations
#
SWITCH_VARS :=  \
    FEATURE_WIFI_PROVISION_ENABLED \
    FEATURE_DEV_BIND_ENABLED \
    FEATURE_ALCS_ENABLED \
    FEATURE_COAP_COMM_ENABLED \
    FEATURE_DEVICE_MODEL_GATEWAY \
    FEATURE_HTTP2_COMM_ENABLED \
    FEATURE_HTTP_COMM_ENABLED \
    FEATURE_MQTT_COMM_ENABLED \
    FEATURE_MQTT_DIRECT \
    FEATURE_MQTT_SHADOW \
    FEATURE_OTA_ENABLED \
    FEATURE_DEVICE_MODEL_ENABLED \
    FEATURE_SUPPORT_ITLS \
    FEATURE_SUPPORT_TLS

#SWITCH_VARS += $(shell grep -o 'FEATURE_[_A-Z0-9]*' $(FEATURE_DEFCONFIG_FILES)|cut -d: -f2|uniq)
SWITCH_VARS := $(sort $(SWITCH_VARS))

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval GLOBAL_CFLAGS += -D$(subst FEATURE_,,$(v)))) \
)

# FEATURE_DEVICE_MODEL_ENABLED
ifeq (y,$(strip $(DEVICE_MODEL_ENABLED)))
    ifeq (y,$(strip $(DEVICE_MODEL_GATEWAY)))
        GLOBAL_CFLAGS += -DCONFIG_SDK_THREAD_COST=1
    endif
endif

# FEATURE_HTTP2_COMM_ENABLED
ifeq (y,$(strip $(HTTP2_COMM_ENABLED)))
    GLOBAL_CFLAGS := $(filter-out -DFORCE_SSL_VERIFY,$(GLOBAL_CFLAGS))
endif
