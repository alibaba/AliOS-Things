include $(CURDIR)/src/tools/internal_make_funcs.mk

SWITCH_VARS := \
    FEATURE_MQTT_COMM_ENABLED \
    FEATURE_MQTT_SHADOW \
    FEATURE_MQTT_DIRECT \
    FEATURE_COAP_COMM_ENABLED \
    FEATURE_COAP_DTLS_SUPPORT \
    FEATURE_ALCS_ENABLED \
    FEATURE_OTA_ENABLED \
    FEATURE_OTA_SIGNAL_CHANNEL \
    FEATURE_OTA_FETCH_CHANNEL \
    FEATURE_MQTT_ID2_AUTH \
    FEATURE_MQTT_ID2_CRYPTO \
    FEATURE_MQTT_ID2_ENV \
    FEATURE_HTTP_COMM_ENABLED \
    FEATURE_HTTP2_COMM_ENABLED \
	FEATURE_SUPPORT_TLS \
	FEATURE_SUPPORT_ITLS \
    FEATURE_SUBDEVICE_ENABLED \
	FEATURE_SDK_ENHANCE \
    FEATURE_CM_ENABLED \
    FEATURE_DM_ENABLED \
    FEATURE_SERVICE_OTA_ENABLED \
    FEATURE_SERVICE_COTA_ENABLED \
    FEATURE_SUPPORT_PRODUCT_SECRET \

$(foreach v, \
    $(SWITCH_VARS), \
    $(if $(filter y,$($(v))), \
        $(eval CFLAGS += -D$(subst FEATURE_,,$(v)))) \
)
ifeq (y,$(strip $(FEATURE_SDK_ENHANCE)))
	FEATURE_CM_ENABLED := y
	FEATURE_DM_ENABLED := y
	CFLAGS += -DCM_ENABLED
	CFLAGS += -DDM_ENABLED
	ifeq (y,$(strip $(FEATURE_DEV_PRODUCT_GW)))
		CFLAGS += -DCONFIG_DM_DEVTYPE_GATEWAY
		CFLAGS += -DCM_SUPPORT_MULTI_THREAD
	else
		CFLAGS += -DCONFIG_DM_DEVTYPE_SINGLE	
	endif
endif # FEATURE_SDK_ENHANCE
ifeq (y,$(strip $(FEATURE_HTTP2_COMM_ENABLED)))
    ifneq (y,$(strip $(FEATURE_SUPPORT_TLS)))
        $(error FEATURE_HTTP2_COMM_ENABLED = y requires FEATURE_SUPPORT_TLS = y!)
	endif
endif # HTTP2

ifeq (y,$(strip $(FEATURE_OTA_ENABLED)))
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
	ifeq (y,$(strip $(FEATURE_SDK_ENHANCE)))
		CFLAGS += -DSERVICE_OTA_ENABLED
	endif #enable ota on enhance sdk
endif # OTA Enabled

ifeq (y,$(strip $(FEATURE_SUBDEVICE_ENABLED)))
ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
$(error FEATURE_SUBDEVICE_ENABLED = y requires FEATURE_MQTT_COMM_ENABLED = y!)
endif
CFLAGS += -DSUBDEVICE_SUPPORT
ifeq (gateway,$(strip $(FEATURE_SUBDEVICE_STATUS)))
CFLAGS += -DGATEWAY_SUPPORT
endif # FEATURE_SUBDEVICE_STATUS
endif # FEATURE_SUBDEVICE_ENABLED

ifeq (MQTT,$(strip $(FEATURE_SUBDEVICE_CHANNEL)))
ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
$(error FEATURE_SUBDEVICE_CHANNEL = MQTT requires FEATURE_MQTT_COMM_ENABLED = y!)
endif
CFLAGS += -DSUBDEV_VIA_MQTT
endif # FEATURE_SUBDEVICE_CHANNEL

ifeq (y,$(strip $(FEATURE_CM_ENABLED)))
ifneq (y,$(strip $(FEATURE_OTA_ENABLED)))
$(error FEATURE_CM_ENABLED = y requires FEATURE_OTA_ENABLED = y!)
endif
ifeq (y,$(strip $(FEATURE_CMP_VIA_MQTT_DIRECT)))
ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
$(error FEATURE_CMP_VIA_MQTT_DIRECT = y requires FEATURE_MQTT_COMM_ENABLED = y!)
endif
CFLAGS += -DCMP_VIA_MQTT_DIRECT
endif #FEATURE_CMP_VIA_MQTT_DIRECT
ifeq (n,$(strip $(FEATURE_CMP_VIA_MQTT_DIRECT)))
CFLAGS += -DCMP_VIA_CLOUN_CONN
ifeq (MQTT,$(strip $(FEATURE_CMP_VIA_CLOUD_CONN)))
ifneq (y,$(strip $(FEATURE_MQTT_COMM_ENABLED)))
$(error FEATURE_CMP_VIA_CLOUD_CONN = MQTT requires FEATURE_MQTT_COMM_ENABLED = y!)
endif #FEATURE_MQTT_COMM_ENABLED
CFLAGS += -DCMP_VIA_CLOUD_CONN_MQTT
endif #FEATURE_CMP_VIA_CLOUD_CONN
ifeq (COAP,$(strip $(FEATURE_CMP_VIA_CLOUD_CONN)))
ifneq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
$(error FEATURE_CMP_VIA_CLOUD_CONN = COAP requires FEATURE_COAP_COMM_ENABLED = y!)
endif #FEATURE_COAP_COMM_ENABLED
CFLAGS += -DCMP_VIA_CLOUD_CONN_COAP
endif #FEATURE_CMP_VIA_CLOUD_CONN
ifeq (HTTP,$(strip $(FEATURE_CMP_VIA_CLOUD_CONN)))
ifneq (y,$(strip $(FEATURE_HTTP_COMM_ENABLED)))
$(error FEATURE_CMP_VIA_CLOUD_CONN = HTTP requires FEATURE_HTTP_COMM_ENABLED = y!)
endif #FEATURE_HTTP_COMM_ENABLED
CFLAGS += -DCMP_VIA_CLOUD_CONN_HTTP
endif #FEATURE_CMP_VIA_CLOUD_CONN
endif #FEATURE_CMP_VIA_MQTT_DIRECT = n
endif #FEATURE_CM_ENABLED

include build-rules/settings.mk
sinclude $(CONFIG_TPL)

ifeq (,$(filter reconfig distclean,$(MAKECMDGOALS)))
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

ifeq (y,$(strip $(FEATURE_MQTT_DIRECT)))

    ifeq (y,$(strip $(FEATURE_MQTT_ID2_CRYPTO)))
    $(error FEATURE_MQTT_ID2_CRYPTO + FEATURE_MQTT_DIRECT is not supported!)
    endif
	
else   # ifeq (y,$(strip $(FEATURE_MQTT_DIRECT)))
    ifeq (n,$(strip $(FEATURE_SUPPORT_TLS)))
        ifeq (n,$(strip $(FEATURE_SUPPORT_ITLS)))
            $(error FEATURE_SUPPORT_TLS or FEATURE_SUPPORT_ITLS must be selected one or more)
        endif
    endif

endif   # ifeq (y,$(strip $(FEATURE_MQTT_DIRECT)))

ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))
    ifeq (y,$(strip $(FEATURE_SUPPORT_ITLS)))
        $(error FEATURE_SUPPORT_TLS and FEATURE_SUPPORT_ITLS are not supported together!)
    endif
else # ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))
    CFLAGS  += -DIOTX_WITHOUT_TLS
endif # ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))

ifeq (n,$(strip $(FEATURE_SUPPORT_ITLS)))
    CFLAGS  += -DIOTX_WITHOUT_ITLS
endif # ifeq (n,$(strip $(FEATURE_SUPPORT_ITLS)))

ifeq (y,$(strip $(FEATURE_MQTT_ID2_AUTH)))
    ifneq (y,$(strip $(FEATURE_SUPPORT_TLS)))
        ifneq (y,$(strip $(FEATURE_SUPPORT_ITLS)))
            $(error FEATURE_SUPPORT_ITLS or FEATURE_SUPPORT_TLS must be selected one or more)
        endif
    endif
    ifneq (y,$(strip $(FEATURE_SUPPORT_ITLS)))
    $(error FEATURE_MQTT_ID2_AUTH requires FEATURE_SUPPORT_ITLS = n!)
    endif

    ifeq (y,$(strip $(FEATURE_COAP_DTLS_SUPPORT)))
    $(error FEATURE_COAP_DTLS_SUPPORT = y requires FEATURE_MQTT_ID2_AUTH = n!)
    endif
    # ifneq (gcc,$(strip $(CC)))
    # $(error FEATURE_MQTT_ID2_AUTH requires $(CC) equal gcc!)
    # endif

else    # ifeq (y,$(strip $(FEATURE_MQTT_ID2_AUTH)))
    ifeq (y,$(strip $(FEATURE_SUPPORT_ITLS)))
    $(error FEATURE_MQTT_ID2_AUTH = n requires FEATURE_SUPPORT_ITLS = n!)
    endif

    ifeq (y,$(strip $(FEATURE_MQTT_ID2_CRYPTO)))
    $(error FEATURE_MQTT_ID2_CRYPTO = y requires FEATURE_MQTT_ID2_AUTH = y!)
    endif

endif   # ifeq (y,$(strip $(FEATURE_MQTT_ID2_AUTH)))

ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
else    # ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))

    ifeq (y,$(strip $(FEATURE_COAP_DTLS_SUPPORT)))
    $(error FEATURE_COAP_DTLS_SUPPORT = y requires FEATURE_COAP_COMM_ENABLED = y!)
    endif

endif   # ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))

ifeq (pre,$(strip $(FEATURE_MQTT_ID2_ENV)))
CFLAGS  += -DTEST_ID2_PRE
else
ifeq (daily,$(strip $(FEATURE_MQTT_ID2_ENV)))
CFLAGS  += -DTEST_ID2_DAILY
endif
endif
endif

ifeq (y,$(strip $(FEATURE_DM_ENABLED)))
    ifneq (y,$(strip $(FEATURE_CM_ENABLED)))
    $(error FEATURE_DM_ENABLED = y requires FEATURE_CM_ENABLED = y!)
    endif
endif

ifeq (y,$(strip $(FEATURE_SERVICE_OTA_ENABLED)))
    ifneq (y,$(strip $(FEATURE_CM_ENABLED)))
    $(error FEATURE_SERVICE_OTA_ENABLED = y requires FEATURE_CM_ENABLED = y!)
    endif
    CFLAGS  += -DSERVICE_OTA_ENABLED

    ifeq (y,$(strip $(FEATURE_SERVICE_COTA_ENABLED)))
    ifneq (y,$(strip $(FEATURE_SERVICE_OTA_ENABLED)))
    $(error FEATURE_SERVICE_COTA_ENABLED = y requires FEATURE_SERVICE_OTA_ENABLED = y!)
    endif
    CFLAGS  += -DSERVICE_COTA_ENABLED
endif
endif

ifeq (y,$(strip $(FEATURE_SUPPORT_PRODUCT_SECRET)))
    CFLAGS  += -DSUPPORT_PRODUCT_SECRET
endif

ifeq (y,$(strip $(FEATURE_COAP_COMM_ENABLED)))
    ifeq (y,$(strip $(FEATURE_ALCS_ENABLED)))
    $(error FEATURE_COAP_COMM_ENABLED=y conflicts with FEATURE_ALCS_ENABLED=y!)
    endif
endif

SUBDIRS += src/platform
SUBDIRS += samples
ifeq (y,$(strip $(FEATURE_SDK_UNIT_TEST)))
SUBDIRS += tests
endif
ifeq (y,$(strip $(FEATURE_SUPPORT_TLS)))
SUBDIRS += src/thirdparty/tls
endif
ifeq (y,$(strip $(FEATURE_MQTT_ID2_AUTH)))
SUBDIRS += src/thirdparty/tfs
endif
SUBDIRS += src/tools/linkkit_tsl_convert
