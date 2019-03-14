NAME := board_wm_w600_kit

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board wm_w600_kit
MODULE             := 1062
HOST_ARCH          := Cortex-M3
HOST_MCU_FAMILY    := mcu_wm_w600
SUPPORT_MBINS      := no
HOST_MCU_NAME      := wm_w600
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr lwip

GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += KV_CONFIG_PARTITION=7

$(NAME)_SOURCES += config/k_config.c \
				   config/partition_conf.c \
                   startup/board.c   \
                   startup/startup.c  \
                   startup/wm_osal_alios.c \
                   drivers/heap_dram.c

$(NAME)_SOURCES += startup/startup_gcc.s

GLOBAL_LDFLAGS  += -T board/wm_w600_kit/link_w600.ld

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_WM_W600
CONFIG_SYSINFO_DEVICE_NAME := WM_W600

GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -mcpu=cortex-m3 -mthumb -mfloat-abi=soft

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_NAME)/mkimage.mk

