TARGET      := sdk-testsuites
HDR_REFS    := src build-rules
SRCS        := $(wildcard $(TOP_DIR)/$(MODULE_NAME)/*.c)
SRCS        += $(TOP_DIR)/build-rules/misc/cut.c
CFLAGS      := $(filter-out -ansi,$(CFLAGS))
LDFLAGS     := -Bstatic
LDFLAGS     += -liot_sdk
LDFLAGS     += -liot_platform

ifneq (,$(filter -DMQTT_ID2_AUTH,$(CFLAGS)))
ifneq (,$(filter -DON_DAILY,$(CFLAGS)))
LDFLAGS     += -ltfs
else
LDFLAGS     += -ltfs_online
endif
ifeq (,$(filter -DIOTX_WITHOUT_ITLS,$(CFLAGS)))
LDFLAGS     += -litls -liot_tfs -liot_sdk
endif
endif

ifeq (,$(filter -DIOTX_WITHOUT_TLS,$(CFLAGS)))
LDFLAGS     += -liot_tls
endif

DEPENDS     += src/platform
