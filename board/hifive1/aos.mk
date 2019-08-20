NAME := board_hifive1

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board hifive1
MODULE             := hifive1
HOST_ARCH          := RISC-V
HOST_MCU_FAMILY    := mcu_e310
SUPPORT_MBINS      := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += STDIO_UART=0 CONFIG_NO_TCPIP

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_HIFIVE1
CONFIG_SYSINFO_DEVICE_NAME   := HIFIVE1

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DDEBUG_CONFIG_NORMAL_PRT=1
