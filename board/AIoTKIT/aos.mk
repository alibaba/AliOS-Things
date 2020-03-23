NAME := board_AIoTKIT

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board AIoTKIT
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32l4xx_cube
SUPPORT_MBINS      := no
HOST_MCU_NAME      := AIoTKIT
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c

$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c \
                   Src/board_drv_led.c     \
                   Src/main.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32l476xx_keil.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32l476xx_iar.s
else
$(NAME)_SOURCES += startup_stm32l476xx.s
endif

GLOBAL_INCLUDES += .    \
                   hal/ \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32L476xx

GLOBAL_DEFINES += STDIO_UART=0

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/AIoTKIT/STM32L476RGTx_FLASH.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/AIoTKIT/stm32l476xx_flash.icf
else
GLOBAL_LDFLAGS += -T board/AIoTKIT/STM32L476RGTx_FLASH.ld
endif

AOS_NETWORK_SAL    ?= y
ifeq (y,$(AOS_NETWORK_SAL))
$(NAME)_COMPONENTS += sal netmgr
module             ?= wifi.mk3060
else
GLOBAL_DEFINES     += CONFIG_NO_TCPIP
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_stm32l476xx_keil.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_AIoTKIT
CONFIG_SYSINFO_DEVICE_NAME   := AIoTKIT

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32L476RGTx
