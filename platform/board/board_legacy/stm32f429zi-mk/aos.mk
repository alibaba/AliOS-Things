NAME := board_stm32f429zi-mk

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := configuration for board stm32f429zi-mk
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32f4xx_cube
SUPPORT_MBINS       := yes
HOST_MCU_NAME      := STM32F429ZIT6
ENABLE_VFP         := 1
ENABLE_USPACE      := 1
APP_FORMAT         := bin

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init cli ulog

ifneq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES += aos/aos.c
endif

$(NAME)_SOURCES += src/stm32f4xx_hal_msp.c \
                   src/board.c \
                   src/soc_init.c \
                   src/eth.c \
                   src/gpio.c \
                   src/usart.c \
                   src/dma.c \
                   src/main.c \
                   src/ethernetif.c \
                   src/httpserver-netconn.c \
                   src/board_partition.c \
				   aos/soc_impl.c \

GLOBAL_INCLUDES += include \
                   include/lwip

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += src/startup_stm32f429xx_keil.s
$(NAME)_LINK_FILES := src/startup_stm32f429xx_keil.o
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES    += src/startup_stm32f429xx_iar.s
else
$(NAME)_SOURCES    += src/startup_stm32f429xx.S
endif

GLOBAL_CFLAGS  += -DSTM32F429xx

ifeq ($(RHINO_CONFIG_UTCPIP),y)
GLOBAL_DEFINES += LWIP_TASK_CANCEL
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/stm32f429zi-mk/STM32F429ZITx.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/board_legacy/stm32f429zi-mk/STM32F429.icf
else
ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T platform/board/board_legacy/stm32f429zi-mk/STM32F429ZITx_FLASH.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T platform/board/board_legacy/stm32f429zi-mk/STM32F429ZITx_FLASH_kernel.ld
else ifeq ($(MBINS),app)
ifeq ($(ENABLE_USPACE),1)
GLOBAL_LDFLAGS += -T platform/board/board_legacy/stm32f429zi-mk/$(MBINS_APP).ld
else
GLOBAL_LDFLAGS += platform/board/board_legacy/stm32f429zi-mk/STM32F429ZITx_FLASH_app.ld
endif
endif
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f429-mk
CONFIG_SYSINFO_DEVICE_NAME   := f429-mk

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

