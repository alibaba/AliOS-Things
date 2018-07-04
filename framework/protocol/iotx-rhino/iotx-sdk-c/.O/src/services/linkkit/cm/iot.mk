LIBA_TARGET     := libiot_cmp.a
HDR_REFS        += src/utils
HDR_REFS        += src/sdk-impl
HDR_REFS        += src/services/linkkit/cm/inc
HDR_REFS        += src/protocol/mqtt/Link-MQTT

CFLAGS          += -DCMP_SUPPORT_TOPIC_DISPATCH \

