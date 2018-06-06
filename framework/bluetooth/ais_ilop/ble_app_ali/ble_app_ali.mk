NAME := ble_app_ali

$(NAME)_MBINS_TYPE := kernel

GLOBAL_INCLUDES += .

$(NAME)_COMPONENTS := bluetooth.ais_ilop.ali_lib protocols.bluetooth

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/ble_app_ali.a
