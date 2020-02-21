NAME := board_stm32l432kc-nucleo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board stm32l432kc-nucleo
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32l4xx_cube
SUPPORT_MBINS      := no
HOST_MCU_NAME      := STM32L432KC-Nucleo
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init
$(NAME)_COMPONENTS-$(BSP_SUPPORT_EXTERNAL_MODULE) = external_module

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c

$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c \
                   Src/main.c
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32l432xx_keil.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32l432xx_iar.s
else
$(NAME)_SOURCES += startup_stm32l432xx.s
endif

GLOBAL_INCLUDES += .    \
                   hal/ \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32L432xx

GLOBAL_DEFINES += STDIO_UART=0

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/stm32l432kc-nucleo/STM32L432KCUx_FLASH.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/board_legacy/stm32l432kc-nucleo/STM32L432.icf
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/stm32l432kc-nucleo/STM32L432KCUx_FLASH.ld
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32l432xx_keil.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_432-nucleo
CONFIG_SYSINFO_DEVICE_NAME   := 432-nucleo

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32L432KCUx

# Iar project support: OGChipSelectEditMenu
$(NAME)_IAR_OGCMENU = STM32L432KC ST STM32L432KC
