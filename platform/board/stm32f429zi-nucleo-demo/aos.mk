NAME := board_stm32f429zi-nucleo-demo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := configuration for board stm32f429zi-nucleo-demo
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32f4xx_cube
SUPPORT_MBINS       := no
HOST_MCU_NAME      := STM32F429ZIT6
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init netmgr


$(NAME)_SOURCES += config/partition_conf.c \
                   config/k_config.c \
                   startup/board.c \
                   startup/startup.c \
                   drivers/mbmaster_hal/port_serial.c

$(NAME)_SOURCES += drivers/Src/stm32f4xx_hal_msp.c \
                   drivers/Src/can.c               \
                   drivers/Src/timer.c             \
                   drivers/Src/i2c.c               \
                   drivers/Src/eth.c               \
                   drivers/Src/gpio.c              \
                   drivers/Src/pwm.c               \
                   drivers/Src/usart.c             \
                   drivers/Src/usb_otg.c           \
                   drivers/Src/dma.c               \
                   drivers/Src/dac.c               \
                   drivers/Src/spi.c

$(NAME)_SOURCES += drivers/drv/board_drv_led.c
ywss_support    ?= 0

GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=32768 #32kb
GLOBAL_DEFINES += KV_CONFIG_BLOCK_SIZE_BITS=14 #(1 << 14) = 16kb

ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
$(NAME)_SOURCES    += drivers/ethernetif.c
$(NAME)_SOURCES    += drivers/httpserver-netconn.c
endif

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup/startup_stm32f429xx_keil.s
$(NAME)_LINK_FILES := startup/startup_stm32f429xx_keil.o
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES    += startup/startup_stm32f429xx_iar.s
else
$(NAME)_SOURCES    += startup/startup_stm32f429xx.s
endif

GLOBAL_INCLUDES += config \
                   drivers/Inc \
                   drivers

GLOBAL_CFLAGS  += -DSTM32F429xx -DCENTRALIZE_MAPPING

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/stm32f429zi-nucleo-demo/STM32F429ZITx.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/stm32f429zi-nucleo-demo/STM32F429.icf
else
GLOBAL_LDFLAGS += -T platform/board/stm32f429zi-nucleo-demo/STM32F429ZITx_FLASH.ld
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f429-nucleo
CONFIG_SYSINFO_DEVICE_NAME   := f429-nucleo

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32F429ZITx

# Iar project support: OGChipSelectEditMenu
$(NAME)_IAR_OGCMENU = STM32F429ZI   ST STM32F429ZI
