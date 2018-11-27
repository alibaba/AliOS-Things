NAME := ble

$(NAME)_SOURCES-y := breeze_hal_ble.c breeze_hal_os.c

$(NAME)_COMPONENTS-y := network.bluetooth.bt network.bluetooth.profile

enhanced_auth ?= 1
ifeq ($(enhanced_auth),1)
$(NAME)_SOURCES-y += breeze_hal_sec.c
$(NAME)_SOURCES-y += aes.c
$(NAME)_SOURCES-y += aes_mbed.c
$(NAME)_INCLUDES += include/mbedtls
endif

GLOBAL_DEFINES-y += CONFIG_BT_SMP
