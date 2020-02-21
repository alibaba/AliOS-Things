NAME := board_mk3239

JTAG := jlink_swd

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board mk3239
MODULE             := 3239
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32f4xx
HOST_MCU_VARIANT   := STM32F412
SUPPORT_MBINS      := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init network

WLAN_CHIP              := 43438
WLAN_CHIP_REVISION     := A1
WLAN_CHIP_FAMILY       := 4343x
WLAN_CHIP_FIRMWARE_VER := 7.45.98.38

BT_CHIP                := 43438
BT_CHIP_REVISION       := A1
BT_CHIP_XTAL_FREQUENCY := 26MHz

BUS := SDIO

$(NAME)_SOURCES          := board.c wifi_nvram.c
$(NAME)_PREBUILT_LIBRARY := MiCO.$(MODULE).$(TOOLCHAIN_NAME).a

GLOBAL_INCLUDES += ./
GLOBAL_DEFINES  += HSE_VALUE=26000000

GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=0

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3239
CONFIG_SYSINFO_DEVICE_NAME   := MK3239

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS += -L $($(NAME)_LOCATION)

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -DSTM32F412 -mcpu=cortex-m4 -mthumb -mfloat-abi=soft

WIFI_FIRMWARE_SECTOR_START := 2      #0x2000
BT_PATCH_SECTOR_START      := 256     #0x100000

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=
