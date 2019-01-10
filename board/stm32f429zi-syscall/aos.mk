NAME := board_stm32f429zi-syscall

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board stm32f429zi-syscall
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32f4xx_syscall
SUPPORT_MBINS       := yes
HOST_MCU_NAME      := STM32F429ZIT6
ENABLE_VFP         := 1
ENABLE_USPACE      := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) osal_aos lwip

$(NAME)_SOURCES += src/stm32f4xx_hal_msp.c \
                   src/board.c \
                   src/soc_init.c \
                   src/eth.c \
                   src/gpio.c \
                   src/usart.c \
                   src/dma.c \
                   src/main.c \
                   src/uspace_init.c \
                   src/ethernetif.c \
                   src/httpserver-netconn.c

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

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/stm32f429zi-syscall/STM32F429ZITx.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/stm32f429zi-syscall/STM32F429.icf
else
ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T board/stm32f429zi-syscall/STM32F429ZITx_FLASH.ld
else ifeq ($(MBINS),kernel)
GLOBAL_LDFLAGS += -T board/stm32f429zi-syscall/STM32F429ZITx_FLASH_kernel.ld
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T board/stm32f429zi-syscall/$(MBINS_APP).ld
endif
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f429-syscall
CONFIG_SYSINFO_DEVICE_NAME   := f429-syscall

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

