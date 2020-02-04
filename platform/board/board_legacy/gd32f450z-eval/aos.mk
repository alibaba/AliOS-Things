NAME := board_gd32f450z-eval

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board gd32f450z-eval
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_gd32f4xx
SUPPORT_MBINS      := no
HOST_MCU_NAME      := GD32F450ZKT6
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr

GLOBAL_DEFINES += GD32F450 CONFIG_NO_TCPIP

$(NAME)_SOURCES += config/k_config.c \
				   config/partition_conf.c \
                   drivers/gd32f4xx_it.c \
                   drivers/gd32f450z_eval.c \
                   startup/board.c   \
                   startup/startup.c

$(NAME)_SOURCES += startup/startup_gd32f450_gcc.s

GLOBAL_LDFLAGS  += -T platform/board/board_legacy/gd32f450z-eval/gd32f4xx_flash.ld

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/  \

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_gd32f450z-eval
CONFIG_SYSINFO_DEVICE_NAME := gd32f450z-eval

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

