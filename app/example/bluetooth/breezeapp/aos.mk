NAME := breezeapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := breezeapp
$(NAME)_SOURCES := breezeapp.c

ble = 1
bz_en_auth = 1
bz_en_awss = 0

$(NAME)_COMPONENTS := breeze cli

GLOBAL_DEFINES += DEBUG
#GLOBAL_DEFINES += CONFIG_BLE_LINK_PARAMETERS
GLOBAL_DEFINES += BUILD_AOS

BREEZEAPP_CONFIG_EN_OTA ?= 0
bz_en_ota ?= $(BREEZEAPP_CONFIG_EN_OTA)
ifeq ($(bz_en_ota),1)
ifeq ($(bz_en_auth), 0)
$(error OTA need authentication, please set "bz_en_auth = 1")
endif
GLOBAL_DEFINES += CONFIG_AIS_OTA
$(NAME)_COMPONENTS += ota_ble
endif

bz_secure_adv ?= 0
ifeq ($(bz_secure_adv),1)
GLOBAL_DEFINES += CONTINUE_BEL_ADV
endif

bz_model_encry ?= 0
ifeq ($(bz_model_encry), 1)
GLOBAL_DEFINES += CONFIG_MODEL_SECURITY
endif

GLOBAL_INCLUDES += ../
