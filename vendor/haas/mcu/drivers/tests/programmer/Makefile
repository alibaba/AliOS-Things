cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.S))
obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.S=.o)

obj-y += ../../utils/hexdump/ ../../utils/hwtimer_list/

obj-y += ../../platform/drivers/norflash/ ../../platform/drivers/ana/

ifeq ($(CHIP_HAS_USB),1)
obj-y += ../../platform/drivers/usb/usb_dev/
CFLAGS_sys_api_programmer.o += -Iplatform/drivers/usb/usb_dev/inc
endif

#ccflags-y += -I$(obj)/../../utils/hexdump -I$(obj)/../../utils/boot_struct -I$(obj)/../../utils/rom_utils
ccflags-y += -Iutils/hexdump -Iutils/hwtimer_list -Iutils/boot_struct -Iutils/rom_utils

CFLAGS_sys_api_programmer.o += -Iplatform/drivers/ana

ifneq ($(DEBUG_PORT),)
CFLAGS_sys_api_programmer.o += -DDEBUG_PORT=$(DEBUG_PORT)
endif

ifeq ($(FLASH_SECURITY_REGISTER),1)
CFLAGS_flash_programmer.o += -DFLASH_SECURITY_REGISTER
CFLAGS_sys_api_programmer.o += -DFLASH_SECURITY_REGISTER
endif

ifeq ($(FLASH_REMAP),1)
CFLAGS_flash_programmer.o += -DFLASH_REMAP
CFLAGS_sys_api_programmer.o += -DFLASH_REMAP
endif

ifeq ($(PROGRAMMER_LOAD_SIMU),1)
CFLAGS_sys_api_programmer.o += -DPROGRAMMER_LOAD_SIMU
endif

ifeq ($(NO_UART_IOMUX),1)
CFLAGS_sys_api_programmer.o += -DNO_UART_IOMUX
endif

ifeq ($(FORCE_ANALOG_I2C),1)
CFLAGS_sys_api_programmer.o += -DFORCE_ANALOG_I2C
endif

