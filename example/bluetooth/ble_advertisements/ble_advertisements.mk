NAME := App_Ble_Advertisements

$(NAME)_SOURCES := ble_advertisements.c

$(NAME)_COMPONENTS := bluetooth.smartbt \
                      bluetooth.mk3239.low_energy

GLOBAL_DEFINES := QC_TEST_BLUETOOTH_ENABLE
