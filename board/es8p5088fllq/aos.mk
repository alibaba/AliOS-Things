NAME := board_es8p5088fllq

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board es8p5088fllq
MODULE             := 1062
HOST_ARCH          := Cortex-M0
HOST_MCU_FAMILY    := mcu_es8p508x
SUPPORT_MBINS      := no
HOST_MCU_NAME      := es8p508x
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub

$(NAME)_SOURCES += config/k_config.c \
                   startup/board.c   \
                   startup/startup.c \
                   drivers/irqhandler.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup/startup_keil.s
$(NAME)_LINK_FILES := startup/startup_keil.o
GLOBAL_LDFLAGS += -L --scatter=board/es8p5088fllq/es8p508x.sct
endif

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/  \

# Keil project support
$(NAME)_KEIL_VENDOR = ES
$(NAME)_KEIL_DEVICE = ES8P508xFLXX
