NAME := breeze

$(NAME)_MBINS_TYPE := kernel

ifeq ($(HOST_ARCH), Cortex-M4)
LIB_DIR := cortex-m4
else ifeq ($(HOST_ARCH), ARM968E-S)
LIB_DIR := arm968es
else ifeq ($(HOST_ARCH), xtensa)
LIB_DIR := xtensa
else
$(error "not find correct platform!")
endif

GLOBAL_INCLUDES += include hal/include

$(NAME)_COMPONENTS := digest_algorithm chip_code

breeze_hal_use_ref ?= 1
ifeq (1, $(breeze_hal_use_ref))
$(NAME)_COMPONENTS += bluetooth.breeze.hal.ble
endif

$(NAME)_PREBUILT_LIBRARY := lib/$(LIB_DIR)/breeze.a

