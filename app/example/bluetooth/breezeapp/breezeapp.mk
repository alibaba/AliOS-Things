NAME := breezeapp

$(NAME)_SOURCES := breezeapp.c

GLOBAL_DEFINES += BLE_APP_RECONFIG_AISILOP

ble = 1

$(NAME)_COMPONENTS := yloop bluetooth.breeze cli

GLOBAL_DEFINES += DEBUG #CONFIG_BT_SMP CONFIG_AIS_OTA

ifeq ($(continue_ble_adv),1)
GLOBAL_DEFINES += CONTINUE_BEL_ADV
endif

ifeq ($(append_user_adv),1)
GLOBAL_DEFINES += APPEND_USER__ADV
endif
