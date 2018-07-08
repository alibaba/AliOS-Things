NAME := aisilopapp

$(NAME)_SOURCES := aisilopapp.c

GLOBAL_DEFINES += BLE_APP_RECONFIG_AISILOP

ble = 1

$(NAME)_COMPONENTS := protocols.bluetooth yloop bluetooth.ais_ilop.ble_app_ali bluetooth.profile cli

GLOBAL_CFLAGS += -DCONFIG_BT_DEVICE_NAME=\"BleAisDevice\"

GLOBAL_DEFINES += DEBUG CONFIG_BT_SMP CONFIG_AIS_OTA
