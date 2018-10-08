NAME := linkkit_sdk_c

$(NAME)_COMPONENTS := middleware/linkkit/hal

GLOBAL_INCLUDES += \
    include/exports \
    include/imports \
    include

#from src/board/config.rhino.make
GLOBAL_CFLAGS  +=

GLOBAL_DEFINES  += \
    BUILD_AOS \
    CONFIG_DM_SUPPORT_LOCAL_CONN \

GLOBAL_DEFINES   += \
    CONFIG_HTTP_AUTH_TIMEOUT=500 \
    CONFIG_MID_HTTP_TIMEOUT=500 \
    CONFIG_GUIDER_AUTH_TIMEOUT=500 \
    CONFIG_MQTT_TX_MAXLEN=640 \
    CONFIG_MQTT_RX_MAXLEN=1200 \
    WITH_MQTT_ZIP_TOPIC=1 \
    WITH_MQTT_SUB_SHORTCUT=1

ROOT_DIR := ../../../
GLOBAL_INCLUDES += $(ROOT_DIR)middleware/linkkit/sdk-c/src/sdk-impl \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/utils/digest \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/utils \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/utils/misc \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/infra/log \
$(ROOT_DIR)middleware/linkkit/sdk-c/include/exports \
$(ROOT_DIR)middleware/linkkit/sdk-c/include/imports \
$(ROOT_DIR)middleware/linkkit/sdk-c/include \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/alcs \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/linkkit/ntp \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/dev_reset \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/dev_bind \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/dev_bind/os \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/dev_bind/utility \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/awss \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/linkkit/dm \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/linkkit/cm \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/ota \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/ota/impl \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/utils/misc  \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/mqtt \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/mqtt/client

#from src/tools/default_settings.mk
GLOBAL_DEFINES  += \
    COAP_OBSERVE_SERVER_SUPPORTED \
    COAP_OBSERVE_CLIENT_SUPPORTED \
    COAP_SERV_MULTITHREAD \
    COAP_USE_PLATFORM_MEMORY COAP_USE_PLATFORM_LOG

#####################################################################
# Configs for component middleware.linkkit.sdk-c
#
# middleware/linkkit/sdk-c/src/infra/utils
#$(NAME)_COMPONENTS += \
#    middleware/linkkit/sdk-c/src/infra/log \
#    middleware/linkkit/sdk-c/src/infra/system \
#    middleware/linkkit/sdk-c/src/sdk-impl \
#    middleware/linkkit/sdk-c/src/ref-impl/hal

ifeq (y,$(FEATURE_SUPPORT_TLS))
#$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/ref-impl/tls
endif

ifeq (y,$(FEATURE_DEV_RESET_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/dev_reset
endif

ifeq (y,$(FEATURE_DEV_BIND_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/dev_bind
endif

ifeq (y,$(FEATURE_NTP_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/linkkit/ntp
endif

ifeq (y,$(FEATURE_WIFI_AWSS_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/awss
endif

ifeq (y,$(FEATURE_MQTT_COMM_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/protocol/mqtt
endif

ifeq (y,$(FEATURE_OTA_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/ota
endif

ifeq (y,$(FEATURE_MQTT_SHADOW))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/shadow
endif

ifeq (y,$(FEATURE_COAP_COMM_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/protocol/coap
endif

ifeq (y,$(FEATURE_HTTP_COMM_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/protocol/http
endif

ifeq (y,$(FEATURE_ALCS_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/protocol/alcs
endif

ifeq (y,$(FEATURE_DEVICE_MODEL_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/linkkit/cm \
    middleware/linkkit/sdk-c/src/services/linkkit/dm
endif

ifeq (y,$(FEATURE_HTTP2_COMM_ENABLED))
$(NAME)_COMPONENTS += \
    middleware/linkkit/sdk-c/src/services/file_upload \
    middleware/linkkit/sdk-c/src/protocol/http2
endif

#####################################################################
# Process dependencies of configurations
#
SWITCH_VARS :=  \
    FEATURE_WIFI_AWSS_ENABLED \
    FEATURE_NTP_ENABLED \
    FEATURE_DEV_RESET_ENABLED \
    FEATURE_DEV_BIND_ENABLED \
    FEATURE_ALCS_ENABLED \
    FEATURE_COAP_COMM_ENABLED \
    FEATURE_COAP_DTLS_SUPPORT \
    FEATURE_ENHANCED_GATEWAY \
    FEATURE_HTTP2_COMM_ENABLED \
    FEATURE_HTTP_COMM_ENABLED \
    FEATURE_MQTT_COMM_ENABLED \
    FEATURE_MQTT_DIRECT \
    FEATURE_MQTT_SHADOW \
    FEATURE_OTA_ENABLED \
    FEATURE_DEVICE_MODEL_ENABLED \
    FEATURE_SUPPORT_ITLS \
    FEATURE_SUPPORT_TLS

SWITCH_VARS += $(shell grep -o 'FEATURE_[_A-Z0-9]*' $(FEATURE_DEFCONFIG_FILES)|cut -d: -f2|uniq)
SWITCH_VARS := $(sort $(SWITCH_VARS))

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval GLOBAL_CFLAGS += -D$(subst FEATURE_,,$(v)))) \
)

# FEATURE_DEVICE_MODEL_ENABLED
ifeq (y,$(strip $(FEATURE_DEVICE_MODEL_ENABLED)))
    ifeq (y,$(strip $(FEATURE_ENHANCED_GATEWAY)))
        GLOBAL_CFLAGS += -DCONFIG_DM_DEVTYPE_GATEWAY
        GLOBAL_CFLAGS += -DCONFIG_SDK_THREAD_COST=1
    else
        GLOBAL_CFLAGS += -DCONFIG_DM_DEVTYPE_SINGLE
    endif
endif

# FEATURE_HTTP2_COMM_ENABLED
ifeq (y,$(strip $(FEATURE_HTTP2_COMM_ENABLED)))
    GLOBAL_CFLAGS := $(filter-out -DFORCE_SSL_VERIFY,$(GLOBAL_CFLAGS))
    ifneq (y,$(strip $(FEATURE_SUPPORT_TLS)))
        $(error FEATURE_HTTP2_COMM_ENABLED = y requires FEATURE_SUPPORT_TLS = y!)
    endif
endif
