NAME := breezeapp

$(NAME)_SOURCES := breezeapp.c

GLOBAL_DEFINES += BLE_APP_RECONFIG_AISILOP

ble = 1

$(NAME)_COMPONENTS := yloop bluetooth.breeze cli

GLOBAL_DEFINES += DEBUG
GLOBAL_DEFINES += CONFIG_BLE_LINK_PARAMETERS
#GLOBAL_DEFINES += CONFIG_BT_SMP

GLOBAL_DEFINES += BUILD_AOS

breeze_ota ?= 0
ifeq ($(breeze_ota),1)
GLOBAL_DEFINES += CONFIG_AIS_OTA
endif

ifeq ($(continue_ble_adv),1)
GLOBAL_DEFINES += CONTINUE_BEL_ADV
endif

ifeq ($(append_user_adv),1)
GLOBAL_DEFINES += APPEND_USER_ADV
endif


GLOBAL_INCLUDES += ../