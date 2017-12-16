NAME := App_Ble_Hello_Sensor

$(NAME)_SOURCES := ble_hello_sensor.c

$(NAME)_COMPONENTS := bluetooth.smartbt \
                      bluetooth.mk3239.low_energy

GLOBAL_DEFINES := QC_TEST_BLUETOOTH_ENABLE
