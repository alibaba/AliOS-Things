NAME := board_stm32l433rc-nucleo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board stm32l433rc-nucleo
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32l4xx_cube
SUPPORT_MBINS      := no
HOST_MCU_NAME      := STM32L433RC-Nucleo
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init
$(NAME)_COMPONENTS-$(BSP_SUPPORT_EXTERNAL_MODULE) = external_module

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c

$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c \
                   Src/main.c
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32l433xx_keil.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32l433xx_iar.s
else
$(NAME)_SOURCES += startup_stm32l433xx.s
endif

GLOBAL_INCLUDES += .    \
                   hal/ \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32L433xx

GLOBAL_DEFINES += STDIO_UART=0

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/stm32l433rc-nucleo/STM32L433.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config STM32L433.icf
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/stm32l433rc-nucleo/STM32L433RCTxP_FLASH.ld
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32l433xx_keil.o
$(NAME)_LINK_FILES += Src/stm32l4xx_hal_msp.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_433-nucleo
CONFIG_SYSINFO_DEVICE_NAME   := 433-nucleo

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32L433RCTx
