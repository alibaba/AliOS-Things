NAME := gd32f307c-eval

JTAG := jlink_swd

$(NAME)_TYPE := kernel
MODULE               := 1062
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := gd32f30x
SUPPORT_BINS         := no
HOST_MCU_VARIANT     := GD32F307VCT6
ENABLE_VFP           := 1

$(NAME)_SOURCES += board.c

GLOBAL_DEFINES += CONFIG_NO_TCPIP

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES += ../../platform/mcu/gd32f30x/driver/Firmware/CMSIS/GD/GD32F30x/Source/ARM/startup_gd32f30x_cl.s
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES += ../../platform/mcu/gd32f30x/driver/Firmware/CMSIS/GD/GD32F30x/Source/IAR/startup_gd32f30x_cl.s  
else
$(NAME)_SOURCES += ../../platform/mcu/gd32f30x/driver/Firmware/CMSIS/GD/GD32F30x/Source/GCC/startup_gd32f30x_cl.S
endif 

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_gd32f307c-eval
CONFIG_SYSINFO_DEVICE_NAME := gd32f307c-eval

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
