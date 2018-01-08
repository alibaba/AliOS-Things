NAME := Lib_Ble_Access_Core_Framework

GLOBAL_INCLUDES += .

$(NAME)_SOURCES := ble_access_core.c \
                    ble_access_core_i.c

$(NAME)_COMPONENTS := framework/bluetooth/bt_smart \
				      framework/bluetooth/low_energy
