NAME := ble

$(NAME)_SOURCES := breeze_hal_ble.c breeze_hal_os.c

$(NAME)_COMPONENTS := network.bluetooth.bt network.bluetooth.profile

enhanced_auth ?= 1
ifeq ($(enhanced_auth),1)
$(NAME)_SOURCES += breeze_hal_sec.c
$(NAME)_SOURCES += aes.c
$(NAME)_SOURCES += aes_mbed.c
$(NAME)_INCLUDES += include/mbedtls
endif

GLOBAL_DEFINES += CONFIG_BT_SMP
