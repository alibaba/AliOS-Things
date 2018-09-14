NAME := breeze

$(NAME)_MBINS_TYPE := kernel

GLOBAL_INCLUDES += include hal/include

$(NAME)_COMPONENTS := digest_algorithm chip_code

$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/breeze.a

breeze_hal_use_ref ?= 1
ifeq (1, $(breeze_hal_use_ref))
$(NAME)_COMPONENTS += bluetooth.breeze.hal.ble
endif
