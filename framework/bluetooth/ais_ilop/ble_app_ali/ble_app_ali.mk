NAME := ble_app_ali

$(NAME)_MBINS_TYPE := kernel

$(NAME)_COMPONENTS := bluetooth.ais_ilop.ali_lib protocols.bluetooth

ifeq ($(HOST_ARCH), Cortex-M4)
LIB_DIR := cortex-m4
else
$(error "not find correct platform!")
endif

GLOBAL_INCLUDES += .

$(NAME)_PREBUILT_LIBRARY := lib/$(LIB_DIR)/ble_app_ali.a
