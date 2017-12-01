NAME := alinkapp

$(NAME)_SOURCES := alink_sample.c
$(NAME)_COMPONENTS := log protocol.alink  cli fota netmgr framework.common

ifneq (,${BINS})
GLOBAL_CFLAGS += -DSYSINFO_OS_BINS
endif
CURRENT_TIME = $(shell /bin/date +%Y%m%d.%H%M)
CONFIG_SYSINFO_APP_VERSION = APP-1.0.0-$(CURRENT_TIME)
$(info app_version:${CONFIG_SYSINFO_APP_VERSION})
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

ifneq ($(ywss),0)
$(NAME)_COMPONENTS += ywss
endif

ifeq ($(at_adapter),1)
$(NAME)_COMPONENTS += at_adapter
LWIP = 1
endif

ifneq (,$(filter linux,$(HOST_MCU_FAMILY)))
gateway ?= 0
else
gateway ?= 1
endif

ifeq ($(gateway),1)

$(NAME)_COMPONENTS += gateway
ifneq (,$(filter linuxhost,$(COMPONENTS)))
DDA ?= 1
endif

ifneq (,$(filter armhflinux,$(COMPONENTS)))
DDA ?= 1
endif

endif

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

MESHDEBUG = 0
ifeq ($(meshdebug), 1)
MESHDEBUG = 1
endif

MESHSUPER = 0
ifeq ($(meshsuper), 1)
MESHSUPER = 1
endif

ifeq ($(DDA),1)
GLOBAL_LDFLAGS += -lreadline -lncurses
$(NAME)_COMPONENTS  += dda
endif

ifeq ($(sds),1)
GLOBAL_DEFINES += CONFIG_SDS
endif
