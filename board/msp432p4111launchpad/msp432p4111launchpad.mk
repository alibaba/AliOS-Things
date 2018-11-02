# 组件名称，建议和当前目录名称保持一致
NAME := msp432p4111launchpad

#It's useless for now, just use this code for no reason
MODULE               := 1062

# The ARCH of the mcu is used on this board
# ARM: ARM968E-S/Cortex-M0/Cortex-M3/Cortex-M4/Cortex-M4F/Cortex-M7/Cortex-R3
# other: armhflinux/xtensa
# csky: ck802/ck803
# MIPS: MIPS-I
# 可以在build目录查看不同的aos_tools_chain_*文件来查看
HOST_ARCH            := Cortex-M4

# board上MCU型号或者所属系列，必须和platform/mcu/ 目录下的对应的目录名称保持一致
HOST_MCU_FAMILY      := msp432p4xx

# 用于多bin编译使用，目前只有kernel和app两种类型，除去example部分，其他暂时全部属于kernel
$(NAME)_TYPE := kernel

# 板卡是否支持多BIN
SUPPORT_BINS         := no

# MCU名称，请和厂商的型号官方名称保持一致，会用于自动生成keil/iar工程时的芯片选择。
HOST_MCU_NAME        := MSP432P4111

# 是否使能VFP
ENABLE_VFP           := 1

# 本目录需要编译的源码
$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c \
                   MSP_EXP432P4111.c \
                   system_msp432p4111.c \
                   board_led_drv.c

GLOBAL_INCLUDES += . \
                   aos/

#Global Defines which is needed by the board driver
GLOBAL_CFLAGS += -D__MSP432P4111__ -DDeviceFamily_MSP432P4x1xI -DSTDIO_UART=0

# AliOS Things support to generate keil/iar project automaticly.
# The following content is optional, which is used for the keil/iar project generate
# Gcc compilation is supported by default

# AliOS Things 支持自动生成keil/iar工程。以下部分为不同编译环境的启动文件和连接脚本
# 该部分为可选实现，但是gcc编译环境为默认支持，即gcc的启动文件和连接脚本必须存在
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
GLOBAL_LDFLAGS += -L --scatter=board/msp432p4111launchpad/msp432p4111.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/msp432p4111launchpad/msp432p4111.icf
else
GLOBAL_LDFLAGS += -T board/msp432p4111launchpad/msp432p4111.lds
endif

# yloop needs sal or lwip, module means the Plug-in module Type
sal ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += linkkit/sdk-c/src/services/mdal/sal
module ?= wifi.mk3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

# It's defined by the developer
CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_startup_msp432p4111launchpad
CONFIG_SYSINFO_DEVICE_NAME := msp432p4111launchpad

#GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
