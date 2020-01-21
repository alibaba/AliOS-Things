NAME := board_gd32f307c-eval

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board gd32f307c-eval
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_gd32f30x
SUPPORT_MBINS      := no
HOST_MCU_NAME      := GD32F307VCT6
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr

GLOBAL_DEFINES += GD32F30X_CL CONFIG_NO_TCPIP

$(NAME)_SOURCES += config/k_config.c \
				   config/partition_conf.c \
                   drivers/gd32f30x_it.c \
                   drivers/gd32f307c_eval.c \
                   startup/board.c   \
                   startup/startup.c

$(NAME)_SOURCES += startup/startup_gd32f30x_cl_gcc.s

GLOBAL_LDFLAGS  += -T platform/board/board_legacy/gd32f307c-eval/gd32f30x_flash.ld

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/  \

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_gd32f307c-eval
CONFIG_SYSINFO_DEVICE_NAME := gd32f307c-eval

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

