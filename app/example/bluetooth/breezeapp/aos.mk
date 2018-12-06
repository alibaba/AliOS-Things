NAME := breezeapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := breezeapp
$(NAME)_SOURCES := breezeapp.c

GLOBAL_DEFINES += BLE_APP_RECONFIG_AISILOP

ble = 1
bz_en_auth = 1
bz_en_awss = 1
bz_en_ota = 0

$(NAME)_COMPONENTS := yloop breeze cli

GLOBAL_DEFINES += DEBUG
GLOBAL_DEFINES += CONFIG_BLE_LINK_PARAMETERS

GLOBAL_DEFINES += BUILD_AOS

bz_en_ota ?= 0
ifeq ($(bz_en_ota),1)
GLOBAL_DEFINES += CONFIG_AIS_OTA
$(NAME)_COMPONENTS += ota_ble
endif

ifeq ($(continue_ble_adv),1)
GLOBAL_DEFINES += CONTINUE_BEL_ADV
endif

ifeq ($(append_user_adv),1)
GLOBAL_DEFINES += APPEND_USER_ADV
endif


GLOBAL_INCLUDES += ../
