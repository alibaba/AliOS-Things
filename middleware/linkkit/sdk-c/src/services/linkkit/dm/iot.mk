LIBA_TARGET := libiot_dm.a

HDR_REFS    += src/infra
HDR_REFS    += src/services/linkkit/cm

TARGET      := dm-example
SRCS        := example/*.c

DEPENDS     += src/ref-impl/hal
DEPENDS     += src/ref-impl/tls
LDFLAGS     += -liot_sdk -liot_hal -liot_tls

LIB_SRCS_PATTERN    := *.c
