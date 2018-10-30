NAME := gd32f350r-eval


$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := gd32f3x0
SUPPORT_BINS         := no
HOST_MCU_VARIANT     := GD32F350RBT6
ENABLE_VFP           := 0

GLOBAL_DEFINES += CONFIG_NO_TCPIP

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += ../../platform/mcu/gd32f3x0/driver/Firmware/CMSIS/GD/GD32F3x0/Source/ARM/startup_gd32f3x0.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += ../../platform/mcu/gd32f3x0/driver/Firmware/CMSIS/GD/GD32F3x0/Source/IAR/startup_gd32f3x0.s  
else
$(NAME)_SOURCES += ../../platform/mcu/gd32f3x0/driver/Firmware/CMSIS/GD/GD32F3x0/Source/GCC/startup_gd32f3x0.s
endif 

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_gd32f350r-eval
CONFIG_SYSINFO_DEVICE_NAME := gd32f350r-eval

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
