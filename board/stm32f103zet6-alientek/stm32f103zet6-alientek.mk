NAME := stm32f103zet6-alientek


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M3
HOST_MCU_FAMILY      := stm32f1xx
SUPPORT_BINS         := no
HOST_MCU_NAME        := STM32F103ZETx
ENABLE_VFP           := 0

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c

$(NAME)_SOURCES += Src/stm32f1xx_hal_msp.c \
                   Src/main.c \
                   Src/sram.c

sal ?= 0
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= gprs.sim800
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_stm32f103xe_keil.s
$(NAME)_LINK_FILES := startup_stm32f103xe_keil.o
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_stm32f103xe_iar.s
else
GLOBAL_CFLAGS += -DUSE_DIRECT_UART_PUTC
$(NAME)_SOURCES += startup_stm32f103xe.s
endif

GLOBAL_INCLUDES += . \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32F103xE -DCENTRALIZE_MAPPING -DHAL_SRAM_MODULE_ENABLED

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/stm32f103zet6-alientek/stm32f103xe_flash.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/stm32f103zet6-alientek/stm32f103xe_flash.icf
else
GLOBAL_LDFLAGS += -T board/stm32f103zet6-alientek/STM32F103ZETx_FLASH.ld
endif



CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_STM32F102ZET6
CONFIG_SYSINFO_DEVICE_NAME := stm32f103zet6-alientek

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
