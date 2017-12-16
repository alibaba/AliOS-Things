NAME := ble_access_core

GLOBAL_INCLUDES += .

$(NAME)_SOURCES := ble_access_core.c \
                   ble_access_core_i.c

$(NAME)_COMPONENTS := bluetooth.smartbt \
                      bluetooth.mk3239.low_energy
