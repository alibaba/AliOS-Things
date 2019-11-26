NAME := board_swm320evb

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board swm320evb
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_swm320
SUPPORT_MBINS      := no
HOST_MCU_NAME      := swm320
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init

$(NAME)_SOURCES += config/k_config.c \
                   config/partition_conf.c \
                   startup/board.c   \
                   startup/startup.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup/startup_keil.s
$(NAME)_LINK_FILES := startup/startup_keil.o
GLOBAL_LDFLAGS += -L --scatter=platform/board/swm320evb/swm320.sct
endif

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/  \

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_SWM320EVB
CONFIG_SYSINFO_DEVICE_NAME   := SWM320
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"


# Keil project support
$(NAME)_KEIL_VENDOR = ARM
$(NAME)_KEIL_DEVICE = ARMCM4
