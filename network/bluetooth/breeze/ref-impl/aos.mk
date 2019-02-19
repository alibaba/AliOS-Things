NAME := breeze_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := This is an AliOS Things based implementation of Breeze HAP APIs..

$(NAME)_SOURCES-y := breeze_hal_ble.c breeze_hal_os.c breeze_hal_sec.c  aes.c

inter_ble_stack ?= 1
ifeq ($(inter_ble_stack),1)
$(NAME)_COMPONENTS-y := bt_host 
endif

$(NAME)_INCLUDES += include/mbedtls

inter_breeze_crypto ?= 1
ifeq ($(inter_breeze_crypto),0)
$(NAME)_COMPONENTS-y += mbedtls 
GLOBAL_DEFINES-y += USE_EXTERNAL_MEBDTLS
else
$(NAME)_SOURCES-y += sha256.c
$(NAME)_SOURCES-y += aes_mbed.c
endif

$(NAME)_COMPONENTS-y += ulog 
ble = 1
