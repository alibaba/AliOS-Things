LIBA_TARGET     := libiot_bind.a

HDR_REFS        := src/infra
HDR_REFS        += src/protocol/mqtt
HDR_REFS        += src/protocol/coap/server
HDR_REFS        += src/protocol/coap/CoAPPacket
HDR_REFS        += src/sdk-impl
HDR_REFS        += src/services/awss
HDR_REFS        += src/services/linkkit/dev_reset
HDR_REFS        += src/services/dev_bind

CFLAGS          += -DAWSS_SUPPORT_DEV_BIND_STATIS
