NAME := board_stm32f091rc-nucleo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board stm32f091rc-nucleo
MODULE             := 1062
HOST_ARCH          := Cortex-M0
HOST_MCU_FAMILY    := mcu_stm32f0xx
SUPPORT_MBINS      := no
HOST_MCU_NAME      := STM32F091RCTx
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) osal_aos kernel_init

$(NAME)_SOURCES += config/k_config.c \
                   config/partition_conf.c \
                   drivers/stm32f0xx_hal_msp.c \
                   drivers/system_stm32f0xx.c \
                   startup/board.c \
                   startup/startup.c
ywss_support ?= 0

GLOBAL_DEFINES += CONFIG_NO_TCPIP

#depends on sal module if select sal function via build option "AOS_NETWORK_SAL=y"
AOS_NETWORK_SAL	?= n

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup/startup_stm32f091xc_keil.s
$(NAME)_LINK_FILES := startup/startup_stm32f091xc_keil.o
$(NAME)_LINK_FILES += drivers/stm32f0xx_hal_msp.o
GLOBAL_LDFLAGS += -L --scatter=board/stm32f091rc-nucleo/stm32f091rc-nucleo.sct
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup/startup_stm32f091xc_iar.s
GLOBAL_LDFLAGS += --config board/stm32f091rc-nucleo/stm32f091xc_flash.icf
else
#GLOBAL_CFLAGS += -DUSE_DIRECT_UART_PUTC
$(NAME)_SOURCES += startup/startup_stm32f091xc.s
GLOBAL_LDFLAGS += -T board/stm32f091rc-nucleo/STM32F091RCTx_FLASH.ld
endif

GLOBAL_INCLUDES += . \
                   config/ \
                   drivers/

#GLOBAL_CFLAGS += -DSTM32F091xC -DCENTRALIZE_MAPPING -DHAL_SRAM_MODULE_ENABLED
GLOBAL_CFLAGS += -DSTM32F091xC

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_stm32f091rc-nucleo
CONFIG_SYSINFO_DEVICE_NAME := NUCLEO-F091RC

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"ARM_Cortex-M0\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_NAME)\"


# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32F091RC

# Iar project support: OGChipSelectEditMenu
$(NAME)_IAR_OGCMENU = STM32F091RC ST STM32F091RC
