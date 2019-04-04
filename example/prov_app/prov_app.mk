NAME := prov_app

$(NAME)_SOURCES := app_entry.c

$(NAME)_COMPONENTS := prov yloop cli

ifeq ($(ERASE), 1)
$(NAME)_DEFINES     += CONFIG_ERASE_KEY
endif

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

ifeq ($(HOST_ARCH), linux)
HOST_NAME := $(HOST_ARCH)
else
HOST_NAME := $(shell echo $(CONFIG_SYSINFO_DEVICE_NAME)|tr A-Z a-z)
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_NAME)/libprov_test.a

GLOBAL_INCLUDES += ./
