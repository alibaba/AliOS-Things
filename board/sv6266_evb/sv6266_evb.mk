NAME := board_sv6266_evb

JTAG := ICEman

$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := ANDES_N10
HOST_MCU_FAMILY      := sv6266
SUPPORT_BINS         := no

GLOBAL_INCLUDES += .
GLOBAL_DEFINES += SV6266

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_SV6266_EVB
CONFIG_SYSINFO_DEVICE_NAME := SV6266_EVB

GLOBAL_CFLAGS += -USYSINFO_OS_VERSION -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -USYSINFO_PRODUCT_MODEL -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -USYSINFO_DEVICE_NAME -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)/board/sv6266_evb

# Global defines

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
$(NAME)_SOURCES := board.c

EXTRA_TARGET_MAKEFILES += $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk

# Define default component testcase set
ifeq (, $(findstring yts, $(BUILD_STRING)))
TEST_COMPONENTS += basic api wifi_hal rhino vcall kv yloop alicrypto cjson digest_algorithm hashtable
endif
