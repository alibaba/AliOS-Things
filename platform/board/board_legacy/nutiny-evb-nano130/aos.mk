NAME := board_nutiny-evb-nano130

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board nutiny-evb-nano130
MODULE             := 1062
HOST_ARCH          := Cortex-M0
HOST_MCU_FAMILY    := mcu_nano130ke3bn
SUPPORT_MBINS      := no
HOST_MCU_NAME      := NANO130KE3BN
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr

GLOBAL_DEFINES += NUMICRO_NANO130
GLOBAL_DEFINES += CONFIG_NO_TCPIP
GLOBAL_DEFINES += TOOLCHAIN_GCC_ARM=1
GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += STDIO_UART_BUADRATE=115200
GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=1024

$(NAME)_SOURCES += config/k_config.c \
				   config/partition_conf.c \
                   startup/board.c   \
                   startup/startup.c

$(NAME)_SOURCES += startup/startup_Nano100Series_gcc.c


GLOBAL_LDFLAGS  += -T platform/board/board_legacy/nutiny-evb-nano130/NANO130.ld

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_NUTINY_NANO130
CONFIG_SYSINFO_DEVICE_NAME := $(NAME)
CONFIG_SYSINFO_OS_VERSION := 100

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"

