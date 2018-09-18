TARGET      := sdk-testsuites
SRCS        := $(wildcard $(TOP_DIR)/$(MODULE_NAME)/*.c)
SRCS        += $(TOP_DIR)/build-rules/misc/cut.c
CFLAGS      := $(filter-out -ansi,$(CFLAGS))

HDR_REFS    := build-rules src/infra
HDR_REFS    += src/services

LDFLAGS     := -Bstatic
LDFLAGS     += -liot_sdk
LDFLAGS     += -liot_hal

ifneq (,$(filter -DSUPPORT_ITLS,$(CFLAGS)))
LDFLAGS += \
            -litls \
            -lid2client \
            -lkm \
            -lplat_gen \
            -lalicrypto \
            -lmbedcrypto \

LDFLAGS     += -liot_sdk
endif

ifeq (,$(filter -DIOTX_WITHOUT_TLS,$(CFLAGS)))
LDFLAGS     += -liot_tls
endif

DEPENDS     += src/ref-impl/hal
