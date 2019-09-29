NAME := breezeapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := breezeapp
$(NAME)_SOURCES := breezeapp.c

$(NAME)_COMPONENTS := breeze cli

BLE = 1

GLOBAL_DEFINES += DEBUG
GLOBAL_DEFINES += BUILD_AOS

ifeq ($(CONFIG_COMP_BZ_OTA), y)
ifneq ($(CONFIG_COMP_BZ_EN_AUTH), y)
$(error OTA need authentication, please set "CONFIG_COMP_BZ_EN_AUTH = y")
endif
GLOBAL_DEFINES += CONFIG_AIS_OTA
$(NAME)_COMPONENTS += ota_ble
endif

ifeq ($(CONFIG_COMP_BZ_SECURE_ADV), y)
GLOBAL_DEFINES += CONTINUE_BEL_ADV
endif

ifeq ($(CONFIG_COMP_BZ_MODEL_SEC), y)
GLOBAL_DEFINES += CONFIG_MODEL_SECURITY
endif

$(NAME)_INCLUDES += ../
