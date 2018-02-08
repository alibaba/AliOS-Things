NAME := board_amebaz_dev

JTAG := jlink_swd

$(NAME)_TYPE := kernel
MODULE               := AmebaZ
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := rtl8710bn
SUPPORT_BINS         := no

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += STDIO_UART=0

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RTL8710BN
CONFIG_SYSINFO_DEVICE_NAME := RTL8710BN


GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
#GLOBAL_CFLAGS += -DSYSINFO_KERNEL_VERSION=\"$(CONFIG_SYSINFO_KERNEL_VERSION)\"
#GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"
#include $(SOURCE_ROOT)/board/rtl8710bn/export-rom_symbol_v01.txt

GLOBAL_CFLAGS += -L $(SOURCE_ROOT)/board/amebaz_dev


# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
