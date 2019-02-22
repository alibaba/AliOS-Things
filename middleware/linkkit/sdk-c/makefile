include project.mk
include make.settings
include src/tools/default_settings.mk
include src/tools/parse_make_settings.mk
include $(RULE_DIR)/funcs.mk

# CFLAGS  += -DINSPECT_MQTT_FLOW

COMP_LIB            := libiot_sdk.a
COMP_LIB_COMPONENTS := \
    src/infra/utils \
    src/infra/log \
    src/infra/system \
    src/sdk-impl \

$(call CompLib_Map, FEATURE_MQTT_COMM_ENABLED,  src/protocol/mqtt)
$(call CompLib_Map, FEATURE_OTA_ENABLED,        src/services/ota)
$(call CompLib_Map, FEATURE_COAP_COMM_ENABLED, \
      src/protocol/coap/client \
      src/protocol/coap/CoAPPacket \
 )
$(call CompLib_Map, FEATURE_HTTP_COMM_ENABLED,  src/protocol/http)
$(call CompLib_Map, FEATURE_MQTT_SHADOW,        src/services/shadow)
$(call CompLib_Map, FEATURE_SUBDEVICE_ENABLED,  src/services/subdev)

$(call CompLib_Map, FEATURE_DEVICE_MODEL_ENABLED, \
    src/services/linkkit/cm \
    src/services/linkkit/dm \
    src/services/linkkit/dev_reset \
)
$(call CompLib_Map, FEATURE_HTTP2_COMM_ENABLED, \
    src/services/http2_stream \
    src/protocol/http2 \
)
$(call CompLib_Map, FEATURE_WIFI_PROVISION_ENABLED, \
    src/protocol/coap/server \
    src/protocol/coap/CoAPPacket \
    src/services/awss \
)
$(call CompLib_Map, FEATURE_DEV_BIND_ENABLED, \
    src/protocol/coap/server \
    src/protocol/coap/CoAPPacket \
    src/services/dev_bind \
)
$(call CompLib_Map, FEATURE_ALCS_ENABLED, \
    src/protocol/coap/server \
    src/protocol/coap/CoAPPacket \
    src/protocol/alcs \
)
$(call CompLib_Map, FEATURE_MAL_ENABLED, \
    src/services/mdal/mal \
)
$(call CompLib_Map, FEATURE_SAL_ENABLED, \
    src/services/mdal/sal \
)

# 'Opt1 = y' and 'Opt2 = y' conflicts with each other
#
$(call Conflict_Relation, FEATURE_SUPPORT_TLS, FEATURE_SUPPORT_ITLS)
$(call Conflict_Relation, FEATURE_MAL_ENABLED, FEATURE_MQTT_COMM_ENABLED)
$(call Conflict_Relation, FEATURE_MAL_ENABLED, FEATURE_SAL_ENABLED)
$(call Conflict_Relation, FEATURE_DEVICE_MODEL_RAWDATA_SOLO, FEATURE_DEPRECATED_LINKKIT)
$(call Conflict_Relation, FEATURE_DEVICE_MODEL_RAWDATA_SOLO, FEATURE_DEVICE_MODEL_GATEWAY)

# 'Opt1 = n' and 'Opt2 = n' conflicts with each other
#
$(call Present1_Relation, FEATURE_MQTT_DIRECT, FEATURE_SUPPORT_TLS)


# 'Opt1 = y' requires 'Opt2 = y' as mandantory support
#
$(call Requires_Relation, FEATURE_MQTT_LOGPOST, FEATURE_MQTT_COMM_ENABLED)
$(call Requires_Relation, FEATURE_MQTT_SHADOW, FEATURE_MQTT_COMM_ENABLED)
$(call Requires_Relation, FEATURE_DEVICE_MODEL_GATEWAY, FEATURE_DEVICE_MODEL_ENABLED)
$(call Requires_Relation, FEATURE_HTTP2_COMM_ENABLED, FEATURE_SUPPORT_TLS)
$(call Requires_Relation, FEATURE_OTA_ENABLED, FEATURE_MQTT_COMM_ENABLED)
$(call Requires_Relation, FEATURE_OTA_ENABLED, FEATURE_UTILS_JSON_PARSER)
$(call Requires_Relation, FEATURE_DEV_BIND_ENABLED, FEATURE_MQTT_COMM_ENABLED)
$(call Requires_Relation, FEATURE_WIFI_PROVISION_ENABLED, FEATURE_DEV_BIND_ENABLED)
$(call Requires_Relation, FEATURE_DEVICE_MODEL_ENABLED, FEATURE_UTILS_LITE_CJSON)
$(call Requires_Relation, FEATURE_DYNAMIC_REGISTER, FEATURE_UTILS_LITE_CJSON)
$(call Requires_Relation, FEATURE_WIFI_PROVISION_ENABLED, FEATURE_UTILS_JSON_PARSER)
$(call Requires_Relation, FEATURE_DEVICE_MODEL_SHADOW, FEATURE_DEVICE_MODEL_ENABLED)
$(call Requires_Relation, FEATURE_DEVICE_MODEL_SUBDEV_OTA, FEATURE_DEVICE_MODEL_GATEWAY)
$(call Requires_Relation, FEATURE_DEVICE_MODEL_SUBDEV_OTA, FEATURE_OTA_ENABLED)

include $(RULE_DIR)/rules.mk
include src/tools/mock_build_options.mk

