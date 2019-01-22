LIBA_TARGET     := libiot_reset.a

HDR_REFS        := src/infra
HDR_REFS        += src/protocol/mqtt

CFLAGS          += -DAWSS_SUPPORT_DEV_RESET_STATIS
