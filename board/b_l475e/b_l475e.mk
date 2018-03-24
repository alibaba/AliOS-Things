NAME := board_b_l475e

JTAG := stlink-v2-1

$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32l475
SUPPORT_BINS         := no

$(NAME)_SOURCES := board.c osa_flash.c

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += RHINO_CONFIG_TICK_TASK=0
GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=1

sal ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif


CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_B-L475E
CONFIG_SYSINFO_DEVICE_NAME := B-L475E

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)/board/b_l475e
endif

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -DSTM32L475xx

WIFI_FIRMWARE_SECTOR_START    := 2      #0x2000
FILESYSTEM_IMAGE_SECTOR_START := 256    #0x100000

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
#EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk

# Define default component testcase set
ifneq (, $(findstring yts, $(BUILD_STRING)))
TEST_COMPONENTS += basic api wifi_hal rhino kv yloop alicrypto cjson digest_algorithm hashtable
endif
