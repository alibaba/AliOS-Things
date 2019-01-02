NAME := board_mk1101

JTAG := jlink_swd

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board mk1101
MODULE             := 1062
HOST_ARCH          := Cortex-M3
HOST_MCU_FAMILY    := mcu_mx1101

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) osal init

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES  += STDIO_UART=0 CONFIG_NO_TCPIP
GLOBAL_DEFINES  += RHINO_CONFIG_TICK_TASK=0 RHINO_CONFIG_WORKQUEUE=0

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MX1101
CONFIG_SYSINFO_DEVICE_NAME   := MX1101

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS += -L $($(NAME)_LOCATION)

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -mcpu=cortex-m3 -mthumb

WIFI_FIRMWARE_SECTOR_START    := 2      #0x2000
FILESYSTEM_IMAGE_SECTOR_START := 256    #0x100000

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES += $(MAKEFILES_PATH)/aos_standard_targets.mk
