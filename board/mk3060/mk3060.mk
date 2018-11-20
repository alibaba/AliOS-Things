NAME := board_mk3060

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := configuration for board mk3060

JTAG := jlink

MODULE          := EMW3060
HOST_ARCH       := ARM968E-S
HOST_MCU_FAMILY := moc108
SUPPORT_MBINS   := no

ifeq ($(AOS_2BOOT_SUPPORT), yes)
$(NAME)_SOURCES := flash_partitions.c
$(NAME)_LIBSUFFIX := _2boot
else
$(NAME)_SOURCES := board.c flash_partitions.c
endif

GLOBAL_INCLUDES += .
GLOBAL_DEFINES  += STDIO_UART=0

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3060
CONFIG_SYSINFO_DEVICE_NAME   := MK3060

# OTA Board config
# 0:OTA_RECOVERY_TYPE_DIRECT 1:OTA_RECOVERY_TYPE_ABBACK 2:OTA_RECOVERY_TYPE_ABBOOT
AOS_SDK_2BOOT_SUPPORT := yes
ifeq ($(AOS_2BOOT_SUPPORT), yes)
GLOBAL_CFLAGS         += -DAOS_OTA_RECOVERY_TYPE=1
GLOBAL_CFLAGS         += -DAOS_OTA_2BOOT_CLI
endif
GLOBAL_CFLAGS         += -DAOS_OTA_BANK_SINGLE

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
#GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

GLOBAL_LDS_INCLUDES += $(SOURCE_ROOT)/board/mk3060/memory.ld.S

GLOBAL_2BOOT_LDS_INCLUDES = $(SOURCE_ROOT)/board/mk3060/memory_2boot.ld.S

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
