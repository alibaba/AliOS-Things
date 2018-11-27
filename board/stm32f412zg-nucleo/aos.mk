NAME := stm32f412zg-nucleo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := configuration for board stm32f412zg-nucleo
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := stm32f4xx_cube
SUPPORT_MBINS      := no
HOST_MCU_NAME      := STM32F412ZGTX
ENABLE_VFP         := 1

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c

$(NAME)_SOURCES += Src/stm32f4xx_hal_msp.c \
                   Src/gpio.c               \
                   Src/usart.c             \
                   Src/i2c.c               \
                   Src/usb_otg.c           \
                   Src/main.c              \
                   Src/dma.c

$(NAME)_SOURCES += drv/board_drv_led.c

GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=262144 #256kb
GLOBAL_DEFINES += CONFIG_AOS_KV_BLK_BITS=17 #(1 << 17) = 128kb

AOS_NETWORK_SAL    ?= y
ifeq (y,$(AOS_NETWORK_SAL))
$(NAME)_COMPONENTS += linkkit/sdk-c/src/services/mdal/sal network.netmgr
module             ?= wifi.mk3060
endif

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup_stm32f412zx_keil.s
$(NAME)_LINK_FILES := startup_stm32f412zx_keil.o
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES    += startup_stm32f412xx_iar.s
else
$(NAME)_SOURCES    += startup_stm32f412zx.s
endif

GLOBAL_INCLUDES += .    \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32F412Zx -DCENTRALIZE_MAPPING

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/stm32f412zg-nucleo/stm32f412zx.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/stm32f412zg-nucleo/STM32L412.icf
else
GLOBAL_LDFLAGS += -T board/stm32f412zg-nucleo/STM32F412ZGTx_FLASH.ld
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f412-nucleo
CONFIG_SYSINFO_DEVICE_NAME   := f412-nucleo

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
