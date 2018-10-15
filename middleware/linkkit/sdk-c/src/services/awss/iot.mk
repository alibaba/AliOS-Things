LIBA_TARGET     := libiot_awss.a

HDR_REFS        := src/infra
HDR_REFS        += src/protocol/mqtt
HDR_REFS        += src/protocol/coap_alcs
HDR_REFS        += src/services/dev_bind
HDR_REFS        += src/sdk-impl

CFLAGS          += -DAWSS_SUPPORT_SMARTCONFIG \
                   -DAWSS_SUPPORT_SMARTCONFIG_WPS \
                   -DAWSS_SUPPORT_AHA \
                   -DAWSS_SUPPORT_ADHA \
                   -DAWSS_SUPPORT_APLIST
