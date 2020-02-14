NAME := board_evkbimxrt1050

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board evkbimxrt1050
MODULE             := 1050
HOST_ARCH          := Cortex-M7
HOST_MCU_FAMILY    := mcu_mimxrt1052impl

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MIMXRT1052
CONFIG_SYSINFO_DEVICE_NAME   := MIMXRT1052

GLOBAL_DEFINES += SKIP_SYSCLK_INIT
GLOBAL_DEFINES += XIP_EXTERNAL_FLASH
GLOBAL_DEFINES += XIP_BOOT_HEADER_ENABLE
GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=8192
GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=0x100000
GLOBAL_DEFINES += KV_CONFIG_BLOCK_SIZE_BITS=18

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"
GLOBAL_CFLAGS += -DCPU_MIMXRT1052DVL6B

GLOBAL_INCLUDES += ./ \
                   boot

$(NAME)_SOURCES := board.c                                 \
                   clock_config.c                          \
                   pin_mux.c                               \
                   boot/evkbimxrt1050_flexspi_nor_config.c \
                   boot/evkbimxrt1050_mini_bl.c            \
                   boot/evkbimxrt1050_sdram_ini_dcd.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += arm/startup_MIMXRT1052.S
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/evkbimxrt1050/arm/MIMXRT1052xxxxx_ram.sct
else
# Toolchain related source
$(NAME)_SOURCES += gcc/startup_MIMXRT1052.S
GLOBAL_LDFLAGS += -T  platform/board/board_legacy/evkbimxrt1050/gcc/MIMXRT1052xxxxx_sdram_txt.ld

endif

TEST_COMPONENTS += certificate_test

