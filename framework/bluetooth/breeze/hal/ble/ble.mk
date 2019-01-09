NAME := breeze_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := This is an AliOS Things based implementation of Breeze HAP APIs..

$(NAME)_SOURCES := breeze_hal_ble.c breeze_hal_os.c

#$(NAME)_COMPONENTS := bluetooth.bt_host 

enhanced_auth ?= 1
ifeq ($(enhanced_auth),1)
$(NAME)_SOURCES += breeze_hal_sec.c
$(NAME)_SOURCES += aes.c
$(NAME)_SOURCES += aes_mbed.c
$(NAME)_INCLUDES += include/mbedtls
endif

GLOBAL_DEFINES-y += MBEDTLS_AES_ROM_TABLES
