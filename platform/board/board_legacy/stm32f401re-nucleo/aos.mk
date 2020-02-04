NAME := board_stm32f401re-nucleo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board stm32f401re-nucleo
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_stm32f4xx_cube
SUPPORT_MBINS      := no
HOST_MCU_NAME      := STM32F401RET
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

ifneq ($(ENABLE_USPACE),1)
$(NAME)_SOURCES += aos/aos.c
endif

$(NAME)_SOURCES += aos/board.c \
                   aos/soc_init.c \
				   aos/soc_impl.c \

$(NAME)_SOURCES += Src/stm32f4xx_hal_msp.c \
                   Src/main.c              \
                   Src/stm32f4xx_it.c

ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup_stm32f401xe_keil.s
$(NAME)_LINK_FILES := startup_stm32f401xe_keil.o
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES    += startup_stm32f401xe_iar.s
else
$(NAME)_SOURCES    += startup_stm32f401xe.s
endif

GLOBAL_INCLUDES += .    \
                   aos/ \
                   Inc/

GLOBAL_CFLAGS += -DSTM32F401xE -DSRAM1_SIZE_MAX=0x18000 -DCENTRALIZE_MAPPING

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/stm32f401re-nucleo/stm32f401xe.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/board_legacy/stm32f401re-nucleo/STM32F401.icf
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/stm32f401re-nucleo/STM32F401RETx_FLASH.ld
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_f401-nucleo
CONFIG_SYSINFO_DEVICE_NAME   := f401-nucleo

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = STMicroelectronics
$(NAME)_KEIL_DEVICE = STM32F401RETx
