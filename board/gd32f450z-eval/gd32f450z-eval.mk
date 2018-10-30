NAME := gd32f450z-eval


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := gd32f4xx
SUPPORT_BINS         := no
HOST_MCU_VARIANT     := GD32F450ZKT6
ENABLE_VFP           := 1

$(NAME)_SOURCES += board.c

GLOBAL_DEFINES += CONFIG_NO_TCPIP

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += ../../platform/mcu/gd32f4xx/driver/Firmware/CMSIS/GD/GD32F4xx/Source/ARM/startup_gd32f450.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += ../../platform/mcu/gd32f4xx/driver/Firmware/CMSIS/GD/GD32F4xx/Source/IAR/startup_gd32f450.s  
else
$(NAME)_SOURCES += ../../platform/mcu/gd32f4xx/driver/Firmware/CMSIS/GD/GD32F4xx/Source/GCC/startup_gd32f450.S
endif 

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_gd32f450z-eval
CONFIG_SYSINFO_DEVICE_NAME := gd32f450z-eval

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
