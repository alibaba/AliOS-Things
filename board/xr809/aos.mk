NAME := board_xr809

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board xr809
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_xr871
SUPPORT_MBINS      := no
HOST_MCU_NAME      := xr871
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr lwip cli yloop fatfs

GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_MMC
GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT
GLOBAL_DEFINES += RHINO_CONFIG_TICKS_PER_SECOND=1000

$(NAME)_SOURCES += config/k_config.c \
                   config/partition_conf.c \
                   drivers/oled.c \
                   drivers/ssd1306.c \
                   startup/cli_ext.c \
                   startup/cli_mem.c \
                   startup/cli_upgrade.c \
                   startup/board.c   \
                   startup/startup.c

$(NAME)_SOURCES += startup/startup_gcc.s

$(NAME)_ASMFLAGS += -c -x assembler-with-cpp

include $(SOURCE_ROOT)/platform/mcu/xr871/config.mk
ifneq ($(no_with_xip),1)
GLOBAL_LDFLAGS += -T board/xr809/xr809-xip.ld
else
GLOBAL_LDFLAGS += -T board/xr809/xr809.ld
endif

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_XR809
CONFIG_SYSINFO_DEVICE_NAME := XR809

CURRENT_TIME = $(shell /bin/date +%Y%m%d.%H%M)
define get-os-version
"AOS-R"-$(CURRENT_TIME)
endef

CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_XR871
CONFIG_SYSINFO_DEVICE_NAME := XR871

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
#GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"Cortex-M4F\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"
GLOBAL_CFLAGS += -DSYSINFO_BOARD=\"$(MODULE)\"

EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_NAME)/mkimage.mk
