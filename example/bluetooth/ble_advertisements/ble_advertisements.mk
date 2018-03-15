NAME := ble_Advertisements

$(NAME)_SOURCES := ble_advertisements.c

ble = 1

$(NAME)_COMPONENTS := bluetooth.ble_app_framework yloop

GLOBAL_CFLAGS += -DCONFIG_BT_DEVICE_NAME=\"AosBleAdvertisementsSampleDevice\"
