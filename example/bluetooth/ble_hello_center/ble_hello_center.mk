NAME := App_Ble_Hello_Center

$(NAME)_SOURCES := ble_hello_center.c \
                   ble_hello_peripheral.c \
                   ble_command_line.c \
                   ble_wlan_coexist.c

$(NAME)_COMPONENTS := bluetooth.smartbt \
                      bluetooth.mk3239.low_energy

GLOBAL_DEFINES := QC_TEST_BLUETOOTH_ENABLE
