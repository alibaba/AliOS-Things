LIBA_TARGET     := libiot_bind.a

HDR_REFS        := src/infra
HDR_REFS        += src/protocol/mqtt
HDR_REFS        += src/protocol/alcs
HDR_REFS        += src/services/awss
HDR_REFS        += src/services/dev_reset

ifeq (,$(filter -DALCS_ENABLED,$(CFLAGS)))
LIB_SRCS_EXCLUDE    := awss_cmp_coap.c
endif
