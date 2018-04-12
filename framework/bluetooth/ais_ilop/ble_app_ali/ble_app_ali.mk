NAME := ble_app_ali

$(NAME)_MBINS_TYPE := kernel

$(NAME)_COMPONENTS := bluetooth.ais_ilop.ali_lib protocols.bluetooth

$(NAME)_SOURCES := ali_export.c

GLOBAL_INCLUDES += .
