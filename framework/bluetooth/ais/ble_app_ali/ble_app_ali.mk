NAME := ble_app_ali

$(NAME)_COMPONENTS := bluetooth.ais.ali_lib protocols.bluetooth

$(NAME)_SOURCES := ali_export.c

GLOBAL_INCLUDES += .
