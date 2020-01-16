NAME := board_stm32l496g-discovery

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board stm32l496g-discovery
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32l4xx_cube
SUPPORT_MBINS      := no
HOST_MCU_NAME      := STM32L496G-DISCOVERY

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c

$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c \
                   Src/main.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32l496xx_keil.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32l496xx_iar.s
else
$(NAME)_SOURCES += startup_stm32l496xx.s
endif

GLOBAL_INCLUDES += .    \
                   hal/ \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32L496xx

GLOBAL_DEFINES += STDIO_UART=0

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/stm32l496g-discovery/STM32L496.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/stm32l496g-discovery/STM32L496.icf
else
GLOBAL_LDFLAGS += -T platform/board/stm32l496g-discovery/STM32L496AGIx_FLASH.ld
endif

ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

ifeq (y, $(strip $(AOS_COMP_SAL)))
$(NAME)_COMPONENTS += sal
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32l496xx_keil.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_496-discovery
CONFIG_SYSINFO_DEVICE_NAME   := 496-discovery

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32L496AGIx
