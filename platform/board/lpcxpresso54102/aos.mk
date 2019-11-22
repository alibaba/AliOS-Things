NAME := board_lpcxpresso54102

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board lpcxpresso54102

MODULE          := 1062
HOST_ARCH       := Cortex-M4
HOST_MCU_FAMILY := mcu_lpc54102impl

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LPC54102
CONFIG_SYSINFO_DEVICE_NAME   := lpc54102
GLOBAL_CFLAGS                += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS                += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
#-DCONFIG_NO_TCPIP
GLOBAL_CFLAGS                += -DCPU_LPC54102J512BD64_cm4
GLOBAL_CFLAGS                += -D__USE_CMSIS -D__MULTICORE_MASTER
GLOBAL_CFLAGS                += -D__NEWLIB__

AOS_NETWORK_SAL ?= y
ifeq (y,$(AOS_NETWORK_SAL))
$(NAME)_COMPONENTS += sal
module             ?= wifi.mk3060
endif

GLOBAL_LDFLAGS +=

GLOBAL_INCLUDES += .

$(NAME)_SOURCES :=
$(NAME)_SOURCES += ./board.c
$(NAME)_SOURCES += ./clock_config.c
$(NAME)_SOURCES += ./pin_mux.c

