NAME := board_evkmimxrt1020

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board evkmimxrt1020
MODULE             := 1020
HOST_ARCH          := Cortex-M7
HOST_MCU_FAMILY    := mcu_mimxrt1021impl

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MIMXRT1021
CONFIG_SYSINFO_DEVICE_NAME   := MIMXRT1021

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
GLOBAL_CFLAGS += -DCONFIG_NO_TCPIP
GLOBAL_CFLAGS += -DCPU_MIMXRT1021DAG5A

GLOBAL_INCLUDES += ./ \
                   boot

$(NAME)_SOURCES := board.c                                 \
                   clock_config.c                          \
                   pin_mux.c                               \
                   boot/evkmimxrt1020_flexspi_nor_config.c \
                   boot/evkmimxrt1020_mini_bl.c            \
                   boot/evkmimxrt1020_sdram_ini_dcd.c

TEST_COMPONENTS += certificate_test

