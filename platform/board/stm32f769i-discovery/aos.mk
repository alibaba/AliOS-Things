NAME := board_b_f769

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board b_f769
MODULE             := 1062
HOST_ARCH          := Cortex-M7
HOST_MCU_FAMILY    := mcu_stm32f7xx
SUPPORT_MBINS      := no
HOST_MCU_NAME      := STM32F769I-Discovery

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES += config/k_config.c \
                    config/partition_conf.c  \
                    drivers/stm32f7xx_hal_msp.c    \
                    drivers/stm32f7xx_it.c    \
		            drivers/system_stm32f7xx.c    \
                    startup/board.c   \
                    startup/startup.c
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup/startup_keil.s
$(NAME)_LINK_FILES += startup/startup_keil.o
$(NAME)_LINK_FILES += drivers/stm32f7xx_it.o
$(NAME)_LINK_FILES += drivers/stm32f7xx_hal_msp.o
GLOBAL_LDFLAGS += -L --scatter=platform/board/stm32f769i-discovery/stm32f769i-discovery.sct
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup/startup_iar.s
GLOBAL_LDFLAGS += --config platform/board/stm32f769i-discovery/stm32f769i-discovery.icf
else
GLOBAL_CFLAGS += -DUSE_DIRECT_UART_PUTC
$(NAME)_SOURCES += startup/startup_gcc.s
GLOBAL_LDFLAGS += -T platform/board/stm32f769i-discovery/stm32f769i-discovery.ld
endif
GLOBAL_INCLUDES += . \
                   config/ \
                   drivers/
GLOBAL_DEFINES += STDIO_UART=0 CONFIG_NO_TCPIP
GLOBAL_CFLAGS += -DSTM32F769xx

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_F769-nucleo
CONFIG_SYSINFO_DEVICE_NAME   := f769-nucleo

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
