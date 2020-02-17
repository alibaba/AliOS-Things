NAME := board_asr6501

JTAG := jlink

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := configuration for board asr6501
MODULE             := 1062
HOST_ARCH          := Cortex-M0
HOST_MCU_FAMILY    := mcu_cy8c4147
SUPPORT_MBINS      := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES := board.c \
	               src/asr_board.c \
				   src/delay.c \
				   src/gpio-board.c \
				   src/gpio.c \
				   src/gpio_irq.c \
				   src/low_power.c \
				   src/spi-board.c \
				   src/systime.c \
				   src/timeServer.c \
				   src/utilities.c

manufactory_lora_driver ?= 1

GLOBAL_INCLUDES += . \
				   inc

GLOBAL_DEFINES += CY8C4147

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_CY8C4147
CONFIG_SYSINFO_DEVICE_NAME   := CY8C4147

CONFIG_MANUFACTURER = ASR
CONFIG_DEVICE_MODEL = 6501
CONFIG_VERSION = 4.0

GLOBAL_DEFINES += CONFIG_MANUFACTURER
GLOBAL_DEFINES += CONFIG_DEVICE_MODEL
GLOBAL_DEFINES += CONFIG_VERSION

LORACHIP = sx1262

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
GLOBAL_LDFLAGS += -L $($(NAME)_LOCATION)/asr6501
endif

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)

WIFI_FIRMWARE_SECTOR_START    := 2      #0x2000
FILESYSTEM_IMAGE_SECTOR_START := 256    #0x100000

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=

# Define default component testcase set
ifeq (, $(findstring yts, $(BUILD_STRING)))
GLOBAL_DEFINES  += RHINO_CONFIG_WORKQUEUE=1
TEST_COMPONENTS += basic_test wifi_hal_test rhino_test aos_test kv_test cjson_test
else
GLOBAL_DEFINES  += RHINO_CONFIG_WORKQUEUE=0
endif
