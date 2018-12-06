NAME := breeze_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := This is an AliOS Things based implementation of Breeze HAP APIs..

$(NAME)_SOURCES-y := breeze_hal_ble.c breeze_hal_os.c

$(NAME)_COMPONENTS-y := network.bluetooth.bt network.bluetooth.bt_profile

enhanced_auth ?= 1
ifeq ($(enhanced_auth),1)
$(NAME)_SOURCES-y += breeze_hal_sec.c
$(NAME)_SOURCES-y += aes.c
$(NAME)_SOURCES-y += aes_mbed.c
$(NAME)_INCLUDES += include/mbedtls
endif

en_bt_smp = 1

