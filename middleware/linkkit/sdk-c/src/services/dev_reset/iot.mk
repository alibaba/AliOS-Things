LIBA_TARGET     := libdev_reset.a

CFLAGS          := $(filter-out -Werror,$(CFLAGS))

HDR_REFS        := src/infra
HDR_REFS        += src/protocol/mqtt
