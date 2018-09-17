include project.mk
include make.settings
include src/tools/default_settings.mk
include src/tools/parse_make_settings.mk

# CFLAGS  += -DINSPECT_MQTT_FLOW
# CFLAGS  += -DINSPECT_MQTT_LIST

COMP_LIB            := libiot_sdk.a
COMP_LIB_COMPONENTS := \
    src/infra/utils \
    src/infra/log \
    src/infra/system \
    src/sdk-impl \

$(call CompLib_Map, MQTT_COMM_ENABLED, src/protocol/mqtt)
$(call CompLib_Map, OTA_ENABLED, src/services/uota)
$(call CompLib_Map, MQTT_SHADOW, src/services/shadow)
$(call CompLib_Map, COAP_COMM_ENABLED, src/protocol/coap)
$(call CompLib_Map, HTTP_COMM_ENABLED, src/protocol/http)
$(call CompLib_Map, ALCS_ENABLED, src/protocol/alcs)
$(call CompLib_Map, WIFI_AWSS_ENABLED, src/services/awss)
$(call CompLib_Map, SUBDEVICE_ENABLED, src/services/subdev)
$(call CompLib_Map, SDK_ENHANCE, \
    src/services/linkkit/cm \
    src/services/linkkit/dm)
$(call CompLib_Map, HTTP2_COMM_ENABLED, \
    src/services/file_upload \
    src/protocol/http2)

include $(RULE_DIR)/rules.mk
include src/tools/mock_build_options.mk

