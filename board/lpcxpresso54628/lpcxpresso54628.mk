NAME := board_lpcxpresso54628

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := configuration for board lpcxpresso54628

MODULE          := 54628
HOST_ARCH       := Cortex-M4
HOST_MCU_FAMILY := lpc54628

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LPC54628
CONFIG_SYSINFO_DEVICE_NAME   := LPC54628

GLOBAL_DEFINES += SKIP_SYSCLK_INIT
# GLOBAL_DEFINES += BLK_BITS=18
GLOBAL_DEFINES += STDIO_UART=0

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"
GLOBAL_CFLAGS += -DCONFIG_NO_TCPIP
GLOBAL_CFLAGS += -DCPU_LPC54628J512ET180

GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += ../../platform/mcu/lpc54628/
GLOBAL_INCLUDES += ../../platform/mcu/lpc54628/CMSIS/Include
GLOBAL_INCLUDES += ../../platform/mcu/lpc54628/drivers

$(NAME)_SOURCES :=
$(NAME)_SOURCES += ./board.c
$(NAME)_SOURCES += ./clock_config.c
$(NAME)_SOURCES += ./pin_mux.c

TEST_COMPONENTS += certificate

GLOBAL_CFLAGS += -DTEST_CONFIG_KV_ENABLED=0
GLOBAL_CFLAGS += -DTEST_CONFIG_YLOOP_ENABLED=1

