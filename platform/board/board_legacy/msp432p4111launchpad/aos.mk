NAME := board_msp432p4111launchpad

#It's useless for now, just use this code for no reason
MODULE := 1062

# The ARCH of the mcu is used on this board
# ARM: ARM968E-S/Cortex-M0/Cortex-M3/Cortex-M4/Cortex-M4F/Cortex-M7/Cortex-R3
# other: armhflinux/xtensa
# csky: ck802/ck803
# MIPS: MIPS-I
$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board msp432p4111launchpad
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_msp432p4xx
SUPPORT_MBINS      := no
HOST_MCU_NAME      := MSP432P4111
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init network
$(NAME)_COMPONENTS-$(BSP_SUPPORT_EXTERNAL_MODULE) = external_module

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c        \
                   MSP_EXP432P4111.c     \
                   system_msp432p4111.c  \
                   board_led_drv.c

GLOBAL_INCLUDES += . \
                   aos/

#Global Defines which is needed by the board driver
GLOBAL_CFLAGS += -D__MSP432P4111__ -DDeviceFamily_MSP432P4x1xI -DSTDIO_UART=0

# AliOS Things support to generate keil/iar project automaticly.
# The following content is optional, which is used for the keil/iar project generate
# Gcc compilation is supported by default

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_msp432p4111_uvision.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_msp432p4111_ewarm.c
else
$(NAME)_SOURCES += startup_msp432p4111_gcc.c
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_msp432p4111_uvision.o
endif

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/msp432p4111launchpad/msp432p4111.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/board_legacy/msp432p4111launchpad/msp432p4111.icf
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/msp432p4111launchpad/msp432p4111.lds
endif

# It's defined by the developer
CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_startup_msp432p4111launchpad
CONFIG_SYSINFO_DEVICE_NAME   := msp432p4111launchpad

#GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
