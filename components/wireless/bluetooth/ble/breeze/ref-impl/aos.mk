NAME := breeze_hal

$(NAME)_COMPONENTS-y := chip_code
$(NAME)_COMPONENTS-y += ulog
$(NAME)_COMPONENTS-$(CONFIG_COMP_BZ_HAL_BLESTACK) += bt_host
$(NAME)_COMPONENTS-$(CONFIG_COMP_BZ_HAL_CRYPTO_MEBDTLS) += mbedtls

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := This is an AliOS Things based implementation of Breeze HAP APIs..

$(NAME)_SOURCES-y := breeze_hal_ble.c breeze_hal_os.c breeze_hal_sec.c  aes.c


$(NAME)_INCLUDES += include/mbedtls \
                    ../include      \
                    ../core/include 
ifeq ($(CONFIG_COMP_BZ_HAL_CRYPTO_MEBDTLS), y)
GLOBAL_DEFINES-y += USE_EXTERNAL_MEBDTLS
else
$(NAME)_SOURCES-y += sha256.c
$(NAME)_SOURCES-y += aes_mbed.c
endif

