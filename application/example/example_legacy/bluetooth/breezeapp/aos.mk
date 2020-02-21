NAME := breezeapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := breezeapp
$(NAME)_SOURCES := ble_breeze_app.c

$(NAME)_COMPONENTS := breeze cli


GLOBAL_DEFINES += DEBUG
GLOBAL_DEFINES += BUILD_AOS

ifeq ($(CONFIG_COMP_BZ_OTA), y)
ifneq ($(EN_AUTH), y)
$(error OTA need authentication, please set "CONFIG_COMP_BZ_EN_AUTH = y")
endif
$(NAME)_COMPONENTS-$(CONFIG_COMP_BZ_OTA) += ota
endif

$(NAME)_INCLUDES += ../
