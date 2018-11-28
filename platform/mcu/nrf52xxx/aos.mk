NAME := nrf52xxx

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := driver & sdk &ble controller for platform/mcu nrf52832 or nrf52840

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += libc rhino kernel.fs.vfs digest_algorithm network.bluetooth.bt

#$(NAME)_LIBSUFFIX := $(PLATFORM_MCU_BOARD)
$(NAME)_LIBSUFFIX := -52832

ifeq ($(HOST_MCU_NAME), nrf52840)
include $(SOURCE_ROOT)platform/mcu/$(PLATFORM_MCU_BOARD)/nrf52840.mk
else ifeq ($(HOST_MCU_NAME), nrf52832)
$(NAME)_COMPONENTS += kernel.fs.kv
include $(SOURCE_ROOT)platform/mcu/$(PLATFORM_MCU_BOARD)/nrf52832.mk
#ifeq ($(HOST_MCU_NAME),nrf52810)
#include nrf52810.mk
endif
