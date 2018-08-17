NAME := msp432p4111launchpad


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := msp432p4xx
SUPPORT_BINS         := no
HOST_MCU_NAME        := MSP432P4111
ENABLE_VFP           := 1

$(NAME)_SOURCES += aos/board_partition.c \
                   aos/soc_init.c \
                   MSP_EXP432P4111.c \
                   system_msp432p4111.c
ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += startup_msp432p4111_uvision.s    
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += startup_msp432p4111_ewarm.c
else
$(NAME)_SOURCES += startup_msp432p4111_gcc.c
endif

GLOBAL_INCLUDES += . \
                   aos/
                   
GLOBAL_CFLAGS += -D__MSP432P4111__ -DDeviceFamily_MSP432P4x1xI

GLOBAL_DEFINES += STDIO_UART=0

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/msp432p4111launchpad/MSP432P4111.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config board/msp432p4111launchpad/MSP432P4111.icf
else
GLOBAL_LDFLAGS += -T board/msp432p4111launchpad/MSP432P4111.lds
endif

sal ?= 1
ifeq (1,$(sal))
$(NAME)_COMPONENTS += sal
module ?= wifi.mk3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif

ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup_msp432p4111_uvision.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_startup_msp432p4111launchpad
CONFIG_SYSINFO_DEVICE_NAME := msp432p4111launchpad

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
