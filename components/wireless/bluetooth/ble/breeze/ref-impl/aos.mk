NAME := breeze_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := This is an AliOS Things based implementation of Breeze HAP APIs..

$(NAME)_SOURCES-y := breeze_hal_ble.c breeze_hal_os.c breeze_hal_sec.c  aes.c

ifeq ($(CONFIG_COMP_BZ_HAL_BLESTACK), y)
$(NAME)_COMPONENTS-y := bt_host
endif

$(NAME)_INCLUDES += include/mbedtls \
                    ../include      \
                    ../core/include 
ifeq ($(CONFIG_COMP_BZ_HAL_CRYPTO_MEBDTLS), y)
$(NAME)_COMPONENTS-y += mbedtls
GLOBAL_DEFINES-y += USE_EXTERNAL_MEBDTLS
else
$(NAME)_SOURCES-y += sha256.c
$(NAME)_SOURCES-y += aes_mbed.c
endif

$(NAME)_COMPONENTS-y += ulog
