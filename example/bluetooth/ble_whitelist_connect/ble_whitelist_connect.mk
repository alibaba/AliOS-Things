NAME := App_Ble_Whitelist_Connect

$(NAME)_SOURCES := ble_command_line.c \
                   ble_whitelist_main.c

$(NAME)_COMPONENTS := bluetooth.smartbt \
                      bluetooth.mk3239.low_energy

GLOBAL_DEFINES := QC_TEST_BLUETOOTH_ENABLE
