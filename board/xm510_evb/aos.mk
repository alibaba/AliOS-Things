NAME := board_xm510

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board xm510
MODULE             := ipcxm510
HOST_ARCH          := Cortex-A5
HOST_MCU_FAMILY    := mcu_xm510
SUPPORT_MBINS      := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) init

$(NAME)_SOURCES := board.c

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += FD_SETSIZE=1024 STDIO_UART=0

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_XM510
CONFIG_SYSINFO_DEVICE_NAME   := XM510

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
