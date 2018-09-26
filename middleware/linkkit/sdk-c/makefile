include project.mk
include make.settings
include src/tools/default_settings.mk
include src/tools/parse_make_settings.mk
include $(RULE_DIR)/funcs.mk

# CFLAGS  += -DINSPECT_MQTT_FLOW
# CFLAGS  += -DINSPECT_MQTT_LIST

COMP_LIB            := libiot_sdk.a
COMP_LIB_COMPONENTS := \
    src/infra/utils \
    src/infra/log \
    src/infra/system \
    src/sdk-impl \

$(call CompLib_Map, FEATURE_MQTT_COMM_ENABLED, src/protocol/mqtt)
$(call CompLib_Map, FEATURE_OTA_ENABLED, src/services/uota)
$(call CompLib_Map, FEATURE_MQTT_SHADOW, src/services/shadow)
$(call CompLib_Map, FEATURE_COAP_COMM_ENABLED, src/protocol/coap)
$(call CompLib_Map, FEATURE_HTTP_COMM_ENABLED, src/protocol/http)
$(call CompLib_Map, FEATURE_ALCS_ENABLED, src/protocol/alcs)
$(call CompLib_Map, FEATURE_NTP_ENABLED, src/services/ntp)
$(call CompLib_Map, FEATURE_DEV_RESET_ENABLED, src/services/dev_reset)
$(call CompLib_Map, FEATURE_DEV_BIND_ENABLED, src/services/dev_bind)
$(call CompLib_Map, FEATURE_WIFI_AWSS_ENABLED, src/services/awss)
$(call CompLib_Map, FEATURE_SUBDEVICE_ENABLED, src/services/subdev)
$(call CompLib_Map, FEATURE_SDK_ENHANCE, \
    src/services/linkkit/cm \
    src/services/linkkit/dm)
$(call CompLib_Map, FEATURE_HTTP2_COMM_ENABLED, \
    src/services/file_upload \
    src/protocol/http2)

# 'Opt1 = y' and 'Opt2 = y' conflicts with each other
#
$(call Conflict_Relation, FEATURE_SUPPORT_TLS, FEATURE_SUPPORT_ITLS)
$(call Conflict_Relation, FEATURE_COAP_COMM_ENABLED, FEATURE_ALCS_ENABLED)
$(call Conflict_Relation, FEATURE_COAP_COMM_ENABLED, FEATURE_WIFI_AWSS_ENABLED)
$(call Conflict_Relation, FEATURE_COAP_COMM_ENABLED, FEATURE_SDK_ENHANCE)
$(call Conflict_Relation, FEATURE_COAP_COMM_ENABLED, FEATURE_DEV_BIND_ENABLED)

# 'Opt1 = y' requires 'Opt2 = y' as mandantory support
#
$(call Requires_Relation, FEATURE_MQTT_DIRECT, FEATURE_MQTT_COMM_ENABLED)
$(call Requires_Relation, FEATURE_MQTT_SHADOW, FEATURE_MQTT_COMM_ENABLED)
$(call Requires_Relation, FEATURE_ENHANCED_GATEWAY, FEATURE_SDK_ENHANCE)
$(call Requires_Relation, FEATURE_COAP_DTLS_SUPPORT, FEATURE_COAP_COMM_ENABLED)
$(call Requires_Relation, FEATURE_HTTP2_COMM_ENABLED, FEATURE_SUPPORT_TLS)

include $(RULE_DIR)/rules.mk
include src/tools/mock_build_options.mk

