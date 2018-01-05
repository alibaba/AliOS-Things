NAME := lib_mico_ble_firmware

# Use Firmware images which are already present
ifeq ($(BT_CHIP_XTAL_FREQUENCY),)
$(NAME)_SOURCES := $(BT_CHIP)$(BT_CHIP_REVISION)/bt_firmware_image.c
else
$(NAME)_SOURCES := $(BT_CHIP)$(BT_CHIP_REVISION)/$(BT_CHIP_XTAL_FREQUENCY)/bt_firmware_image.c
endif
