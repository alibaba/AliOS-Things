NAME := board_hc32l136k8ta

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board hc32l136k8ta
MODULE             := 1062
HOST_ARCH          := Cortex-M0
HOST_MCU_FAMILY    := mcu_hc32l136
SUPPORT_MBINS      := no
HOST_MCU_NAME      := hc32l136
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub

$(NAME)_SOURCES += config/k_config.c \
                   startup/board.c   \
                   startup/startup.c \
                   drivers/interrupts_hc32l136.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup/startup_keil.s
$(NAME)_LINK_FILES := startup/startup_keil.o
GLOBAL_LDFLAGS += -L --scatter=board/hc32l136k8ta/hc32l136.sct
endif

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/  \

# Keil project support
$(NAME)_KEIL_VENDOR = HDSC
$(NAME)_KEIL_DEVICE = HC32L136
