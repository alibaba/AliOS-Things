NAME := prov_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := id2 provisioning sample app

$(NAME)_SOURCES := app_entry.c
$(NAME)_SOURCES += prov_test_id2.c

$(NAME)_COMPONENTS := ls_hal libprov yloop cli

ifeq ($(PROVAPP_CONFIG_ERASE), y)
$(NAME)_DEFINES     += CONFIG_ERASE_KEY
endif

$(NAME)_INCLUDES += ./
