
NAME := board_cc3220slp

MODULE              := 0001
HOST_ARCH           := Cortex-M4
HOST_MCU_FAMILY     := cc3220

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_CC3220
CONFIG_SYSINFO_DEVICE_NAME := CC3220
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -Dgcc -mcpu=cortex-m4 -mthumb -ffunction-sections -fdata-sections -MD -std=c99 -g -O0 -c

GLOBAL_LDFLAGS += 

GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += ../../platform/mcu/cc3220
GLOBAL_INCLUDES += ../../platform/mcu/cc3220/ti/devices

$(NAME)_SOURCES     += ./board.c

