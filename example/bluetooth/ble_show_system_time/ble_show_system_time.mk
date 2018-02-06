NAME := app_ble_show_system_time

$(NAME)_SOURCES := ble_show_system_time.c

ble = 1

$(NAME)_COMPONENTS := bluetooth.ble_app_framework

GLOBAL_CFLAGS += -DCONFIG_BT_DEVICE_NAME=\"AosBleShowSysTimeSampleDevice\"
