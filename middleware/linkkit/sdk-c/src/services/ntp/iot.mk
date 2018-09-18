LIBA_TARGET     := libntp.a

CFLAGS          := $(filter-out -Werror,$(CFLAGS))

HDR_REFS        := src/infra
HDR_REFS        += src/protocol/mqtt
