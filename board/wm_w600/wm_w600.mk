NAME := board_wm_w600

#JTAG := jlink_swd

$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M3
HOST_MCU_FAMILY      := wm_w600

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += STDIO_UART=0 #CONFIG_NO_TCPIP
#GLOBAL_DEFINES += RHINO_CONFIG_TICK_TASK=0 RHINO_CONFIG_WORKQUEUE=0

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_WM_W600
CONFIG_SYSINFO_DEVICE_NAME := WM_W600

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)/board/wm_w600

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config 
else
GLOBAL_LDFLAGS += -T board/wm_w600/link_w600.ld
endif

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -mcpu=cortex-m3 -mthumb -mfloat-abi=soft

#WIFI_FIRMWARE_SECTOR_START    := 2      #0x2000
#FILESYSTEM_IMAGE_SECTOR_START := 256    #0x100000

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/mkimage.mk
