NAME := ble_app_helper_impl

$(NAME)_SOURCES := adv_helper.c init_helper.c gatt_attr_helper.c

$(NAME)_COMPONENTS := bluetooth.mk3239.bt_smart bluetooth.ble_app_framework bluetooth.mk3239.low_energy
