LIBA_TARGET     := libiot_cm.a

CFLAGS          += -DCMP_SUPPORT_TOPIC_DISPATCH

HDR_REFS        += src/infra
HDR_REFS        += src/protocol/mqtt

LIB_SRCS_PATTERN    := *.c
