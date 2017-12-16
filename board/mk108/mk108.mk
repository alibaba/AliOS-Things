NAME := board_mk108

JTAG := jlink

MODULE               := MOC108
HOST_ARCH            := ARM968E-S
HOST_MCU_FAMILY      := moc108
SUPPORT_BINS         := no

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += STDIO_UART=1

CURRENT_TIME = $(shell /bin/date +%Y%m%d.%H%M)
define get-os-version
"AOS-R"-$(CURRENT_TIME)
endef

CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)
CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK108
CONFIG_SYSINFO_DEVICE_NAME := MK108

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)/board/mk108

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
