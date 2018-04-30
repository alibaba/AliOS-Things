NAME := stm32l432kc-nucleo


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32l4xx_cube
SUPPORT_BINS         := no
HOST_MCU_NAME        := STM32L433RC-Nucleo
ENABLE_VFP           := 1

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c
                   
$(NAME)_SOURCES += Src/stm32l4xx_hal_msp.c 
                   
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += CMSIS/Device/ST/STM32L4xx/Source/Templates/arm/startup_stm32l432xx.s    
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += CMSIS/Device/ST/STM32L4xx/Source/Templates/arm/startup_stm32l432xx.s   
else
$(NAME)_SOURCES += startup_stm32l432xx.s
endif

GLOBAL_INCLUDES += . \
                   hal/ \
                   aos/ \
                   Inc/
				   
GLOBAL_CFLAGS += -DSTM32L432xx 

GLOBAL_DEFINES += STDIO_UART=2

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config 
else
GLOBAL_LDFLAGS += -T board/stm32l433rc-nucleo/STM32L433RCTxP_FLASH.ld
endif

sal ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_432-nucleo
CONFIG_SYSINFO_DEVICE_NAME := 432-nucleo

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
