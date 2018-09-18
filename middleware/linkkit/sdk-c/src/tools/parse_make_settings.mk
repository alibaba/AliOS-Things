include $(CURDIR)/src/tools/internal_make_funcs.mk

SWITCH_VARS := $(shell grep -o 'FEATURE_[_A-Z0-9]*' $(TOP_DIR)/make.settings $(TOP_DIR)/src/tools/default_settings.mk|cut -d: -f2|uniq)
SWITCH_VARS := $(sort $(SWITCH_VARS))

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval CFLAGS += -D$(subst FEATURE_,,$(v)))) \
)

ifeq (y,$(strip $(FEATURE_SDK_ENHANCE)))
    CFLAGS += -DCONFIG_CM_VIA_CLOUD_CONN
    CFLAGS += -DCONFIG_CM_SUPPORT_LOCAL_CONN
    CFLAGS += -DCONFIG_DM_SUPPORT_LOCAL_CONN
    ifeq (y,$(strip $(FEATURE_ENHANCED_GATEWAY)))
        CFLAGS += -DCONFIG_DM_DEVTYPE_GATEWAY
        CFLAGS += -DCONFIG_SDK_THREAD_COST=1
    else
        CFLAGS += -DCONFIG_DM_DEVTYPE_SINGLE
    endif
endif # FEATURE_SDK_ENHANCE

ifeq (y,$(strip $(FEATURE_HTTP2_COMM_ENABLED)))
    CFLAGS := $(filter-out -DFORCE_SSL_VERIFY,$(CFLAGS))
    ifneq (y,$(strip $(FEATURE_SUPPORT_TLS)))
        $(error FEATURE_HTTP2_COMM_ENABLED = y requires FEATURE_SUPPORT_TLS = y!)
    endif
endif # HTTP2

ifeq (y,$(strip $(FEATURE_OTA_ENABLED)))
    CFLAGS += -DOTA_WITH_LINKKIT
    CFLAGS += -DOTA_HAL_MODULE
    CFLAGS += -DOTA_RSA_DISABLE
    ifeq (n,$(strip $(FEATURE_SUPPORT_TLS)))
        ifeq (n,$(strip $(FEATURE_SUPPORT_ITLS)))
            $(error FEATURE_SUPPORT_TLS or FEATURE_SUPPORT_ITLS must be selected one or more)
        endif
    endif
    ifeq (MQTT,$(strip $(FEATURE_OTA_SIGNAL_CHANNEL)))
        ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
            $(error FEATURE_OTA_SIGNAL_CHANNEL = MQTT requires FEATURE_MQTT_COMM_ENABLED = y!)
        endif
        CFLAGS += -DOTA_SIGNAL_CHANNEL=1
    else
        ifeq (COAP,$(strip $(FEATURE_OTA_SIGNAL_CHANNEL)))
            ifneq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
                $(error FEATURE_OTA_SIGNAL_CHANNEL = COAP requires FEATURE_COAP_COMM_ENABLED = y!)
            endif
            CFLAGS += -DOTA_SIGNAL_CHANNEL=2
        else
            ifeq (HTTP,$(strip $(FEATURE_OTA_SIGNAL_CHANNEL)))
                CFLAGS += -DOTA_SIGNAL_CHANNEL=4
            else
                $(error FEATURE_OTA_SIGNAL_CHANNEL must be MQTT or COAP or HTTP!)
            endif # HTTP
        endif # COAP
    endif # MQTT
endif # OTA Enabled

include build-rules/settings.mk
sinclude $(CONFIG_TPL)

ifeq (,$(filter reconfig env distclean,$(MAKECMDGOALS)))
ifneq (HTTP,$(strip $(FEATURE_OTA_FETCH_CHANNEL)))
$(error FEATURE_OTA_FETCH_CHANNEL must be HTTP!)
endif

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

ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))
    ifeq (y,$(strip $(FEATURE_SUPPORT_ITLS)))
        $(error FEATURE_SUPPORT_TLS and FEATURE_SUPPORT_ITLS are not supported together!)
    endif
else    # ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))
    CFLAGS  += -DIOTX_WITHOUT_TLS
    CFLAGS  += -DITLS_DOWNLOAD
endif   # ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))

ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))

    ifeq (y,$(strip $(FEATURE_ALCS_ENABLED)))
        $(error FEATURE_COAP_COMM_ENABLED=y conflicts with FEATURE_ALCS_ENABLED=y!)
    endif

    ifeq (y,$(strip $(FEATURE_WIFI_AWSS_ENABLED)))
        $(error FEATURE_COAP_COMM_ENABLED=y conflicts with FEATURE_WIFI_AWSS_ENABLED=y!)
    endif

    ifeq (y,$(strip $(FEATURE_SDK_ENHANCE)))
        $(error FEATURE_COAP_COMM_ENABLED=y conflicts with FEATURE_SDK_ENHANCE=y!)
    endif

else    # ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))

    ifeq (y,$(strip $(FEATURE_COAP_DTLS_SUPPORT)))
    $(error FEATURE_COAP_DTLS_SUPPORT = y requires FEATURE_COAP_COMM_ENABLED = y!)
    endif

endif   # ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))

endif   # ifeq (,$(filter reconfig distclean,$(MAKECMDGOALS)))



SUBDIRS += src/ref-impl/hal
SUBDIRS += examples
SUBDIRS += tests
ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))
SUBDIRS += src/ref-impl/tls
endif
SUBDIRS += src/tools/linkkit_tsl_convert
