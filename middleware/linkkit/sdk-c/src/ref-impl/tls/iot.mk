LIBA_TARGET := libiot_tls.a
CFLAGS      := $(filter-out -ansi,$(CFLAGS))
HDR_REFS    += src/infra

LIB_SRC_PATTERN := *.c */*.c
ifneq (,$(filter -DOTA_ENABLED,$(CFLAGS)))
LIB_SRC_PATTERN += uOTA_required/*.c
HDR_REFS        += src/services/uOTA/inc
endif
