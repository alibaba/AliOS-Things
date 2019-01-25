NAME := board_stm32f103ze

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board stm32f103ze
MODULE             := 1062
HOST_ARCH          := Cortex-M3
HOST_MCU_FAMILY    := mcu_stm32f1xx
SUPPORT_MBINS       := no
HOST_MCU_NAME      := STM32F103ZETx
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) init

$(NAME)_SOURCES += config/k_config.c \
                    config/partition_conf.c  \
                    drivers/sram.c    \
                    drivers/gpio.c    \
                    drivers/stm32f1xx_hal_msp.c    \
                    drivers/stm32f1xx_main.c    \
                    startup/board.c   \
                    startup/startup.c

ywss_support    ?= 0

GLOBAL_DEFINES += CONFIG_NO_TCPIP
GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=32768 #32kb
GLOBAL_DEFINES += KV_CONFIG_BLOCK_SIZE_BITS=14 #(1 << 14) = 16kb

#depends on sal module if select sal function via build option "AOS_NETWORK_SAL=y"
AOS_NETWORK_SAL    ?= n

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup/startup_keil.s
$(NAME)_LINK_FILES := startup/startup_keil.o
GLOBAL_LDFLAGS += -L --scatter=board/stm32f103ze/stm32f103ze.sct
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup/startup_iar.s
GLOBAL_LDFLAGS += --config board/stm32f103ze/stm32f103ze.icf
else
GLOBAL_CFLAGS += -DUSE_DIRECT_UART_PUTC
$(NAME)_SOURCES += startup/startup_gcc.s
GLOBAL_LDFLAGS += -T board/stm32f103ze/stm32f103ze.ld
endif

GLOBAL_INCLUDES += . \
                   config/ \
                   drivers/

GLOBAL_CFLAGS += -DSTM32F103xE -DCENTRALIZE_MAPPING -DHAL_SRAM_MODULE_ENABLED

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f103
CONFIG_SYSINFO_DEVICE_NAME   := stm32f103

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32F103ZE

# Iar project support: OGChipSelectEditMenu
$(NAME)_IAR_OGCMENU = STM32F103ZE ST STM32F103ZE

