NAME := board_stm32f429zi-nucleo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board stm32f429zi-nucleo
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32f4xx_cube
SUPPORT_MBINS       := no
HOST_MCU_NAME      := STM32F429ZIT6
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init netmgr

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c        \
                   mbmaster_hal/port_serial.c

$(NAME)_SOURCES += Src/stm32f4xx_hal_msp.c \
                   Src/can.c               \
                   Src/timer.c             \
                   Src/i2c.c               \
                   Src/eth.c               \
                   Src/gpio.c              \
                   Src/usart.c             \
                   Src/usb_otg.c           \
                   Src/dma.c               \
                   Src/main.c

$(NAME)_SOURCES += drv/board_drv_led.c
ywss_support    ?= 0

GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=32768 #32kb
GLOBAL_DEFINES += KV_CONFIG_BLOCK_SIZE_BITS=14 #(1 << 14) = 16kb

ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
$(NAME)_SOURCES    += ethernetif.c
$(NAME)_SOURCES    += httpserver-netconn.c
$(NAME)_COMPONENTS += lwip
endif

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup_stm32f429xx_keil.s
$(NAME)_LINK_FILES := startup_stm32f429xx_keil.o
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES    += startup_stm32f429xx_iar.s
else
$(NAME)_SOURCES    += startup_stm32f429xx.s
endif

GLOBAL_INCLUDES += .    \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS  += -DSTM32F429xx -DCENTRALIZE_MAPPING

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/stm32f429zi-nucleo/STM32F429ZITx.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/board_legacy/stm32f429zi-nucleo/STM32F429.icf
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/stm32f429zi-nucleo/STM32F429ZITx_FLASH.ld
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f429-nucleo
CONFIG_SYSINFO_DEVICE_NAME   := f429-nucleo

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32F429ZITx

# Iar project support: OGChipSelectEditMenu
$(NAME)_IAR_OGCMENU = STM32F429ZI	ST STM32F429ZI

