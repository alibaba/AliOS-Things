NAME := bleadv

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := Ble adv example.
$(NAME)_SOURCES     := main.c

BLE = 1
EN_BT_SMP = 1

$(NAME)_COMPONENTS  += bt_host cli
GLOBAL_DEFINES      += AOS_NO_WIFI

$(NAME)_INCLUDES += ../
