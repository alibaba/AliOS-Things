LIBA_TARGET := libiot_coap_cloud.a

HDR_REFS    += src/infra

DEPENDS     += src/protocol/coap/CoAPPacket
LDFLAGS     += -liot_coap_CoAPPacket