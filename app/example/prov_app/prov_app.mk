NAME := prov_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := id2 provisioning sample app

$(NAME)_SOURCES := app_entry.c

$(NAME)_COMPONENTS := prov yloop cli

ifeq ($(ERASE), 1)
$(NAME)_DEFINES     += CONFIG_ERASE_KEY
endif

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libprov_test.a

GLOBAL_INCLUDES += ./
