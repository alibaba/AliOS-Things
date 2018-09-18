NAME := bleadv

$(NAME)_SOURCES     := main.c

$(NAME)_INCLUDES    := ../../../network/bluetooth/include \
                       ../../../network/bluetooth/core/include

ble = 1

$(NAME)_COMPONENTS  += network.bluetooth.bt yloop cli
GLOBAL_DEFINES      += AOS_NO_WIFI CONFIG_BT_SMP

GLOBAL_INCLUDES += ../