NAME := board_eml3047

JTAG := jlink_swd

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board eml3047
MODULE             := 1062
HOST_ARCH          := Cortex-M0
HOST_MCU_FAMILY    := mcu_stm32l0xx
HOST_MCU_NAME      := stm32l071kb
SUPPORT_MBINS      := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES := board.c           \
                   src/debug.c       \
                   src/gpio-board.c  \
                   src/spi-board.c   \
                   src/rtc-board.c   \
                   src/delay-board.c \
                   src/eml3047_hw.c  \
                   src/eml3047_it.c  \
                   src/uart-board.c  \
                   src/low_power.c   \
                   src/utilities.c

linkwan         ?= 1
ifeq ($(linkwan), 1)
$(NAME)_SOURCES += src/lorawan_port.c \
                   src/sx1276-eml3047.c

lorawanback      ?= 0
ifeq ($(lorawanback), 0)
$(NAME)_COMPONENTS += lorawan_4_4_2
else ifeq ($(lorawanback), 1)
$(NAME)_COMPONENTS += lorawan_4_4_0
endif

LORACHIP          := sx1276

GLOBAL_DEFINES    += EML3047_LORAWAN
endif

#$(NAME)_LINK_FILES := src/eml3047_it.o

GLOBAL_INCLUDES += .   \
                   inc

GLOBAL_DEFINES += USE_HAL_DRIVER \
                  STM32L071xx

GLOBAL_DEFINES += STDIO_UART=0 CONFIG_NO_TCPIP
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=0 DEBUG_CONFIG_ERRDUMP=0
GLOBAL_DEFINES += USE_FULL_LL_DRIVER USE_B_EML3047

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_EML3047
CONFIG_SYSINFO_DEVICE_NAME   := EML3047

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS  += -L $($(NAME)_LOCATION)

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft

WIFI_FIRMWARE_SECTOR_START    := 2      #0x2000
FILESYSTEM_IMAGE_SECTOR_START := 256    #0x100000

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=
