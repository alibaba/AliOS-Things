NAME := board_ssc1667

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board ssc1667
MODULE             := 1062
HOST_ARCH          := Cortex-M3
HOST_MCU_FAMILY    := mcu_sscp131
SUPPORT_MBINS      := no
HOST_MCU_NAME      := SSCP131
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub

$(NAME)_SOURCES += config/k_config.c \
                   startup/board.c   \
                   startup/startup.c \
                   drivers/irqhandler.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup/startup_keil.s
$(NAME)_LINK_FILES := startup/startup_keil.o
GLOBAL_LDFLAGS += -L --scatter= platform/board/ssc1667/ssc1667.sct
endif

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/  \

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_SSC1667
CONFIG_SYSINFO_DEVICE_NAME   := SSC1667

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = ARM
$(NAME)_KEIL_DEVICE = Cortex-M3
