NAME := board_mk3060

JTAG := jlink

$(NAME)_TYPE := kernel
MODULE               := EMW3060
HOST_ARCH            := ARM968E-S
HOST_MCU_FAMILY      := moc108
SUPPORT_BINS         := no

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += STDIO_UART=0

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3060
CONFIG_SYSINFO_DEVICE_NAME := MK3060


GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
#GLOBAL_CFLAGS += -DSYSINFO_KERNEL_VERSION=\"$(CONFIG_SYSINFO_KERNEL_VERSION)\"
#GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

ifeq ($(BINS),)
GLOBAL_LDS_INCLUDES += $(SOURCE_ROOT)/board/mk3060/memory.ld.S
else ifeq ($(BINS),app)
GLOBAL_LDS_INCLUDES += $(SOURCE_ROOT)/board/mk3060/memory_app.ld.S
else ifeq ($(BINS),framework)
GLOBAL_LDS_INCLUDES += $(SOURCE_ROOT)/board/mk3060/memory_framework.ld.S
else ifeq ($(BINS),kernel)
GLOBAL_LDS_INCLUDES += $(SOURCE_ROOT)/board/mk3060/memory_kernel.ld.S
endif

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
