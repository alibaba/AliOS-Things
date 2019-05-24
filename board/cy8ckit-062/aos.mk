NAME := board_cy8ckit-062

JTAG := jlink

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board cy8ckit-062
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_cy8c6347
SUPPORT_MBINS      := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .

AOS_NETWORK_SAL ?= y
ifeq (y,$(AOS_NETWORK_SAL))
$(NAME)_COMPONENTS += sal netmgr
module             ?= wifi.mk3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

GLOBAL_DEFINES += CY8C6347BZI_BLD53

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_CY8C6347BZI
CONFIG_SYSINFO_DEVICE_NAME   := CY8C6347BZI

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

ifeq ($(COMPILER),armcc)
else ifeq ($(COMPILER),iar)
else
GLOBAL_LDFLAGS += -L $($(NAME)_LOCATION)/cy8ckit-062
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
