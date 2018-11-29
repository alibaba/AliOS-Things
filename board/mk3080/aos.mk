NAME := board_mk3080

JTAG := jlink_swd

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := configuration for board mk3080
MODULE             := AmebaZ
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := rtl8710bn
SUPPORT_MBINS      := no
AOS_SDK_2BOOT_SUPPORT := yes

ifeq ($(AOS_2BOOT_SUPPORT), yes)
$(NAME)_LIBSUFFIX := _2boot
GLOBAL_CFLAGS     += -DAOS_OTA_2BOOT_CLI
else
$(NAME)_SOURCES := board.c
endif

GLOBAL_CFLAGS         += -DAOS_OTA_RECOVERY_TYPE=1
GLOBAL_CFLAGS         += -DAOS_OTA_BANK_SINGLE

$(NAME)_SOURCES += flash_partitions.c
GLOBAL_INCLUDES += .
GLOBAL_DEFINES  += STDIO_UART=0 USE_MX1290

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3080
CONFIG_SYSINFO_DEVICE_NAME   := MK3080


GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
#GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"
#include $(SOURCE_ROOT)/board/rtl8710bn/export-rom_symbol_v01.txt

GLOBAL_CFLAGS += -L $(SOURCE_ROOT)/board/mk3080

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/download.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
