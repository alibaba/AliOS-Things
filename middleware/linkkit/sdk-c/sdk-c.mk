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
    IOTX_NET_INIT_WITH_PK_EXT \
    CM_VIA_CLOUD_CONN \
    CM_VIA_CLOUD_CONN_MQTT \
    CM_SUPPORT_MULTI_THREAD_VIA_HAL \
    CM_SUPPORT_LOCAL_CONN \
    CONFIG_DM_SUPPORT_LOCAL_CONN

GLOBAL_DEFINES   += \
    CONFIG_HTTP_AUTH_TIMEOUT=500 \
    CONFIG_MID_HTTP_TIMEOUT=500 \
    CONFIG_GUIDER_AUTH_TIMEOUT=500 \
    CONFIG_MQTT_TX_MAXLEN=640 \
    CONFIG_MQTT_RX_MAXLEN=1200 \

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
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/linkkit/dm \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/services/linkkit/cm \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/utils/misc  \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/mqtt/Link-MQTT \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/mqtt \
$(ROOT_DIR)middleware/linkkit/sdk-c/src/protocol/mqtt/client

#from src/tools/default_settings.mk
GLOBAL_DEFINES  += _IS_LINKKIT_ \
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

ifeq (y,$(FEATURE_MQTT_COMM_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/protocol/mqtt
endif

ifeq (y,$(FEATURE_OTA_ENABLED))
$(NAME)_COMPONENTS += middleware/uagent/uota
endif

ifeq (y,$(FEATURE_MQTT_SHADOW))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/shadow
endif

ifeq (y,$(FEATURE_WIFI_AWSS_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/awss
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

ifeq (y,$(FEATURE_SUBDEVICE_ENABLED))
$(NAME)_COMPONENTS += middleware/linkkit/sdk-c/src/services/subdev
endif

ifeq (y,$(FEATURE_SDK_ENHANCE))
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
    FEATURE_ALCS_ENABLED \
    FEATURE_CMP_VIA_CLOUD_CONN \
    FEATURE_CMP_VIA_MQTT_DIRECT \
    FEATURE_COAP_COMM_ENABLED \
    FEATURE_COAP_DTLS_SUPPORT \
    FEATURE_ENHANCED_GATEWAY \
    FEATURE_HTTP2_COMM_ENABLED \
    FEATURE_HTTP_COMM_ENABLED \
    FEATURE_MQTT_COMM_ENABLED \
    FEATURE_MQTT_DIRECT \
    FEATURE_MQTT_SHADOW \
    FEATURE_OTA_ENABLED \
    FEATURE_OTA_FETCH_CHANNEL \
    FEATURE_OTA_SIGNAL_CHANNEL \
    FEATURE_SDK_ENHANCE \
    FEATURE_SERVICE_COTA_ENABLED \
    FEATURE_SERVICE_OTA_ENABLED \
    FEATURE_SUBDEVICE_CHANNEL \
    FEATURE_SUBDEVICE_ENABLED \
    FEATURE_SUBDEVICE_STATUS \
    FEATURE_SUPPORT_ITLS \
    FEATURE_SUPPORT_PRODUCT_SECRET \
    FEATURE_SUPPORT_TLS \
    FEATURE_WIFI_AWSS_ENABLED

SWITCH_VARS += $(shell grep -o 'FEATURE_[_A-Z0-9]*' $(FEATURE_DEFCONFIG_FILES)|cut -d: -f2|uniq)
SWITCH_VARS := $(sort $(SWITCH_VARS))

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval GLOBAL_CFLAGS += -D$(subst FEATURE_,,$(v)))) \
)

# FEATURE_SDK_ENHANCE
ifeq (y,$(strip $(FEATURE_SDK_ENHANCE)))
    GLOBAL_CFLAGS += -DCM_ENABLED
    GLOBAL_CFLAGS += -DDM_ENABLED
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

# FEATURE_OTA_ENABLED
ifeq (y,$(strip $(FEATURE_OTA_ENABLED)))
    ifeq (n,$(strip $(FEATURE_SUPPORT_TLS)))
        ifeq (n,$(strip $(FEATURE_SUPPORT_ITLS)))
            $(error FEATURE_SUPPORT_TLS or FEATURE_SUPPORT_ITLS must be selected one or more)
        endif
    endif
    # MQTT
    ifeq (MQTT,$(strip $(FEATURE_OTA_SIGNAL_CHANNEL)))
        ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
            $(error FEATURE_OTA_SIGNAL_CHANNEL = MQTT requires FEATURE_MQTT_COMM_ENABLED = y!)
        endif
        GLOBAL_CFLAGS += -DOTA_SIGNAL_CHANNEL=1
    else
        # COAP
        ifeq (COAP,$(strip $(FEATURE_OTA_SIGNAL_CHANNEL)))
            ifneq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
                $(error FEATURE_OTA_SIGNAL_CHANNEL = COAP requires FEATURE_COAP_COMM_ENABLED = y!)
            endif
            GLOBAL_CFLAGS += -DOTA_SIGNAL_CHANNEL=2
        else
            # HTTP
            ifeq (HTTP,$(strip $(FEATURE_OTA_SIGNAL_CHANNEL)))
                GLOBAL_CFLAGS += -DOTA_SIGNAL_CHANNEL=4
            else
                $(error FEATURE_OTA_SIGNAL_CHANNEL must be MQTT or COAP or HTTP!)
            endif
        endif
    endif
    # Enable ota on enhance sdk
    ifeq (y,$(strip $(FEATURE_SDK_ENHANCE)))
        GLOBAL_CFLAGS += -DSERVICE_OTA_ENABLED
    endif
endif

# FEATURE_SUBDEVICE_ENABLED
ifeq (y,$(strip $(FEATURE_SUBDEVICE_ENABLED)))
    ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
        $(error FEATURE_SUBDEVICE_ENABLED = y requires FEATURE_MQTT_COMM_ENABLED = y!)
    endif
    GLOBAL_CFLAGS += -DSUBDEVICE_SUPPORT
    # FEATURE_SUBDEVICE_STATUS
    ifeq (gateway,$(strip $(FEATURE_SUBDEVICE_STATUS)))
        GLOBAL_CFLAGS += -DGATEWAY_SUPPORT
    endif

    # FEATURE_SUBDEVICE_CHANNEL
    ifeq (MQTT,$(strip $(FEATURE_SUBDEVICE_CHANNEL)))
        ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
            $(error FEATURE_SUBDEVICE_CHANNEL = MQTT requires FEATURE_MQTT_COMM_ENABLED = y!)
        endif
        GLOBAL_CFLAGS += -DSUBDEV_VIA_MQTT
    endif
endif

# FEATURE_SDK_ENHANCE
ifeq (y,$(strip $(FEATURE_SDK_ENHANCE)))
    # FEATURE_CMP_VIA_MQTT_DIRECT
    ifeq (y,$(strip $(FEATURE_CMP_VIA_MQTT_DIRECT)))
        ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
            $(error FEATURE_CMP_VIA_MQTT_DIRECT = y requires FEATURE_MQTT_COMM_ENABLED = y!)
        endif
        GLOBAL_CFLAGS += -DCMP_VIA_MQTT_DIRECT
    endif

    #FEATURE_CMP_VIA_MQTT_DIRECT = n
    ifeq (n,$(strip $(FEATURE_CMP_VIA_MQTT_DIRECT)))
        GLOBAL_CFLAGS += -DCMP_VIA_CLOUN_CONN

        # FEATURE_CMP_VIA_CLOUD_CONN = MQTT
        ifeq (MQTT,$(strip $(FEATURE_CMP_VIA_CLOUD_CONN)))
            #FEATURE_MQTT_COMM_ENABLED
            ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
                $(error FEATURE_CMP_VIA_CLOUD_CONN = MQTT requires FEATURE_MQTT_COMM_ENABLED = y!)
            endif
            GLOBAL_CFLAGS += -DCMP_VIA_CLOUD_CONN_MQTT
        endif

        # FEATURE_CMP_VIA_CLOUD_CONN = COAP
        ifeq (COAP,$(strip $(FEATURE_CMP_VIA_CLOUD_CONN)))
            # FEATURE_COAP_COMM_ENABLED
            ifneq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
                $(error FEATURE_CMP_VIA_CLOUD_CONN = COAP requires FEATURE_COAP_COMM_ENABLED = y!)
            endif
            GLOBAL_CFLAGS += -DCMP_VIA_CLOUD_CONN_COAP
        endif

        # FEATURE_HTTP_COMM_ENABLED = HTTP
        ifeq (HTTP,$(strip $(FEATURE_CMP_VIA_CLOUD_CONN)))
            ifneq (y,$(strip $(FEATURE_HTTP_COMM_ENABLED)))
                $(error FEATURE_CMP_VIA_CLOUD_CONN = HTTP requires FEATURE_HTTP_COMM_ENABLED = y!)
            endif
            GLOBAL_CFLAGS += -DCMP_VIA_CLOUD_CONN_HTTP
        endif
    endif
endif

ifneq (HTTP,$(strip $(FEATURE_OTA_FETCH_CHANNEL)))
    $(error FEATURE_OTA_FETCH_CHANNEL must be HTTP!)
endif

# FEATURE_MQTT_COMM_ENABLED
ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
    ifneq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
        $(error Either CoAP or MQTT required to be y!)
    endif

    $(foreach V,SHADOW DIRECT DIRECT_NOTLS, \
        $(if $(filter y,$(strip $(FEATURE_MQTT_$(V)))), \
            $(error FEATURE_MQTT_$(V) = y requires FEATURE_MQTT_COMM_ENABLED = y!) \
        ) \
    )
endif

# FEATURE_SUPPORT_TLS
ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))
    ifeq (y,$(strip $(FEATURE_SUPPORT_ITLS)))
        $(error FEATURE_SUPPORT_TLS and FEATURE_SUPPORT_ITLS are not supported together!)
    endif
else
    GLOBAL_CFLAGS  += -DIOTX_WITHOUT_TLS
    GLOBAL_CFLAGS  += -DITLS_DOWNLOAD
    GLOBAL_DEFINES += ITLS_DOWNLOAD
endif

ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
    ifeq (y,$(strip $(FEATURE_ALCS_ENABLED)))
        $(error FEATURE_COAP_COMM_ENABLED=y conflicts with FEATURE_ALCS_ENABLED=y!)
    endif
else
    ifeq (y,$(strip $(FEATURE_COAP_DTLS_SUPPORT)))
        $(error FEATURE_COAP_DTLS_SUPPORT = y requires FEATURE_COAP_COMM_ENABLED = y!)
    endif
endif

# src/tools/mock_build_options.mk
SKIP_SSL_VERIFY_MODES = \
    ID2_DAILY \
    ID2_PRE \
    MQTT_DAILY \
    OTA_DAILY \
    ON_DAILY \
    ON_PRE   \

SKIP_MQTT_DIRECT_MODES = \
    MQTT_DAILY \
    OTA_DAILY \

ifneq (,$(filter $(foreach M,$(SKIP_SSL_VERIFY_MODES),-D$(M)),$(GLOBAL_CFLAGS)))
GLOBAL_CFLAGS  := $(filter-out -DFORCE_SSL_VERIFY,$(GLOBAL_CFLAGS))
endif

ifneq (,$(filter $(foreach M,$(SKIP_MQTT_DIRECT_MODES),-DTEST_$(M)),$(GLOBAL_CFLAGS)))
GLOBAL_CFLAGS  := $(filter-out -DMQTT_DIRECT,$(GLOBAL_CFLAGS))
endif
