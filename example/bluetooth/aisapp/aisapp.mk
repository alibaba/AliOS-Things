NAME := aisapp

$(NAME)_SOURCES := aisapp.c

ble = 1

$(NAME)_COMPONENTS := protocols.bluetooth yloop bluetooth.ais.ble_app_ali bluetooth.profile

GLOBAL_CFLAGS += -DCONFIG_BT_DEVICE_NAME=\"BleAisDevice\"

GLOBAL_DEFINES += CONFIG_AIS_NO_OTA DEBUG
