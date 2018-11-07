include $(CURDIR)/src/tools/internal_make_funcs.mk

SWITCH_VARS := $(shell grep -o 'FEATURE_[_A-Z0-9]*' $(TOP_DIR)/Config.in $(TOP_DIR)/make.settings|grep -v FEATURE_SRCPATH|cut -d: -f2|uniq)
SWITCH_VARS := $(sort $(SWITCH_VARS))

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval CFLAGS += -D$(subst FEATURE_,,$(v)))) \
)

ifeq (y,$(strip $(FEATURE_WIFI_PROVISION_ENABLED)))
    CFLAGS += -DAWSS_SUPPORT_APLIST

    ifeq (y,$(strip $(FEATURE_AWSS_SUPPORT_SMARTCONFIG)))
        CFLAGS += -DAWSS_SUPPORT_SMARTCONFIG \
                  -DAWSS_SUPPORT_SMARTCONFIG_WPS
    endif

    ifeq (y,$(strip $(FEATURE_AWSS_SUPPORT_PHONEASAP)))
        CFLAGS := $(filter-out -DAWSS_SUPPORT_AHA,$(CFLAGS))
        CFLAGS += -DAWSS_SUPPORT_AHA
    endif

    ifeq (y,$(strip $(FEATURE_AWSS_SUPPORT_ROUTER)))
        CFLAGS := $(filter-out -DAWSS_SUPPORT_AHA,$(CFLAGS))
        CFLAGS += -DAWSS_SUPPORT_AHA \
                  -DAWSS_SUPPORT_ADHA
    endif

    ifeq (y,$(strip $(FEATURE_AWSS_SUPPORT_ZEROCONFIG)))
        CFLAGS += -DAWSS_DISABLE_ENROLLEE \
                  -DAWSS_DISABLE_REGISTRAR
    endif
endif

ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
    CFLAGS += -DCOAP_DTLS_SUPPORT
endif   # FEATURE_COAP_COMM_ENABLED

ifeq (y,$(strip $(FEATURE_DEVICE_MODEL_ENABLED)))
    ifeq (y,$(strip $(FEATURE_DEVICE_MODEL_GATEWAY)))
        CFLAGS += -DCONFIG_SDK_THREAD_COST=1
    endif
endif # FEATURE_DEVICE_MODEL_ENABLED

ifeq (y,$(strip $(FEATURE_OTA_ENABLED)))
    ifeq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
        CFLAGS += -DOTA_SIGNAL_CHANNEL=1
    else
        ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
            CFLAGS += -DOTA_SIGNAL_CHANNEL=2
        else
            ifeq (y,$(strip $(FEATURE_HTTP_COMM_ENABLED)))
                CFLAGS += -DOTA_SIGNAL_CHANNEL=4
            endif # HTTP
        endif # COAP
    endif # MQTT
endif # FEATURE_OTA_ENABLED

include build-rules/settings.mk
sinclude $(CONFIG_TPL)

SUBDIRS += src/ref-impl/hal
SUBDIRS += examples
SUBDIRS += tests
SUBDIRS += src/ref-impl/tls

ifeq (y,$(strip $(FEATURE_DEVICE_MODEL_ENABLED)))
ifeq (y,$(strip $(FEATURE_DEPRECATED_LINKKIT)))
SUBDIRS += src/tools/linkkit_tsl_convert
endif
endif
