NAME := bleadv

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := Ble adv example.
$(NAME)_SOURCES     := main.c

ble = 1

$(NAME)_COMPONENTS  += network.bluetooth.bt yloop cli
GLOBAL_DEFINES      += AOS_NO_WIFI CONFIG_BT_SMP

GLOBAL_INCLUDES += ../
