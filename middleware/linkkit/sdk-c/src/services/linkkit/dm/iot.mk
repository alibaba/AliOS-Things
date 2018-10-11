LIBA_TARGET := libiot_dm.a

HDR_REFS    += src/infra
HDR_REFS    += src/protocol
HDR_REFS    += src/services

DEPENDS     += src/ref-impl/hal
DEPENDS     += src/ref-impl/tls
LDFLAGS     += -liot_sdk -liot_hal -liot_tls

LIB_SRCS_PATTERN    := *.c server/*.c client/*.c
