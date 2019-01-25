NAME := bleperipheral

$(NAME)_MBINS_TYPE  := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := bleperipheral

$(NAME)_SOURCES     := main.c

ble = 1
en_bt_smp = 1

$(NAME)_COMPONENTS  += bt_host cli bt_profile
GLOBAL_DEFINES      += AOS_NO_WIFI

GLOBAL_INCLUDES += ../
