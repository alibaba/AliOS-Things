NAME := App_Ble_Scan

$(NAME)_SOURCES := ble_scan.c

$(NAME)_COMPONENTS := bluetooth.smartbt \
                      bluetooth.mk3239.low_energy

GLOBAL_DEFINES := QC_TEST_BLUETOOTH_ENABLE
