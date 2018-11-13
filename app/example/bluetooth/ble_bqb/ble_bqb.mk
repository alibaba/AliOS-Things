NAME := ble_bqb

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES     := main.c \
                       bqb.c \
                       bqb_gap.c \
                       bqb_gatt.c \
                       bqb_sm.c \
                       bqb_l2cap.c

$(NAME)_INCLUDES    := ./ \
                       ../../../network/bluetooth/include \
                       ../../../network/bluetooth/core/include

ble = 1

$(NAME)_COMPONENTS  += network.bluetooth.bt yloop cli network.bluetooth.profile
GLOBAL_DEFINES      += AOS_NO_WIFI CONFIG_BT_SMP CONFIG_AOS_CLI CONFIG_BT_GATT_CLIENT

GLOBAL_INCLUDES += ../