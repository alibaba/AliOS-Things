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
endif # HTTP2

ifeq (y,$(strip $(FEATURE_OTA_ENABLED)))
    CFLAGS += -DOTA_WITH_LINKKIT
    CFLAGS += -DOTA_HAL_MODULE
    CFLAGS += -DOTA_RSA_DISABLE
    ifeq (MQTT,$(strip $(FEATURE_OTA_SIGNAL_CHANNEL)))
        CFLAGS += -DOTA_SIGNAL_CHANNEL=1
    else
        ifeq (COAP,$(strip $(FEATURE_OTA_SIGNAL_CHANNEL)))
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

endif   # ifeq (,$(filter reconfig distclean,$(MAKECMDGOALS)))

SUBDIRS += src/ref-impl/hal
SUBDIRS += examples
SUBDIRS += tests
ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))
SUBDIRS += src/ref-impl/tls
endif
SUBDIRS += src/tools/linkkit_tsl_convert
