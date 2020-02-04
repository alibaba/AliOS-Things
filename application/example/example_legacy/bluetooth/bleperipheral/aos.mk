NAME := bleperipheral

$(NAME)_MBINS_TYPE  := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := bleperipheral

$(NAME)_SOURCES     := bleperipheral_app.c

BLE = 1
EN_BT_SMP = 1

$(NAME)_COMPONENTS  += bt_host cli bt_profile
GLOBAL_DEFINES      += AOS_NO_WIFI

$(NAME)_INCLUDES += ../
