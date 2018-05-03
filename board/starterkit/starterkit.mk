NAME := starterkit


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32l4xx
SUPPORT_BINS         := no
HOST_MCU_NAME        := STM32L433RC-Nucleo
ENABLE_VFP           := 1

$(NAME)_SOURCES += board.c board_cli.c st7789.c

GLOBAL_INCLUDES += .

GLOBAL_DEFINES += STDIO_UART=0
GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD

$(NAME)_COMPONENTS += sensor
GLOBAL_DEFINES += AOS_SENSOR_ACC_MIR3_DA217
GLOBAL_DEFINES += AOS_SENSOR_ALS_LITEON_LTR553
GLOBAL_DEFINES += AOS_SENSOR_PS_LITEON_LTR553
GLOBAL_DEFINES += AOS_SENSOR_ACC_SUPPORT_STEP

sal ?= 1
no_tls ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_starterkit
CONFIG_SYSINFO_DEVICE_NAME := starterkit

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
