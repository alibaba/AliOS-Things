NAME := prov_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := id2 provisioning sample app

$(NAME)_SOURCES := app_entry.c

$(NAME)_COMPONENTS := prov yloop cli

ifeq ($(ERASE), 1)
$(NAME)_DEFINES     += CONFIG_ERASE_KEY
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/libprov_test.a

GLOBAL_INCLUDES += ./
