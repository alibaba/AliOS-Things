
NAME := board_lpcxpresso54102

MODULE              := 1062
HOST_ARCH           := Cortex-M4
HOST_MCU_FAMILY     := lpc54102

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LPC54102
CONFIG_SYSINFO_DEVICE_NAME := LPC54102
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
#-DCONFIG_NO_TCPIP
GLOBAL_CFLAGS += -DCPU_LPC54102J512BD64_cm4
GLOBAL_CFLAGS += -D__USE_CMSIS -D__MULTICORE_MASTER
GLOBAL_CFLAGS += -D__NEWLIB__



GLOBAL_LDFLAGS += 

GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += ../../platform/mcu/lpc54102/
GLOBAL_INCLUDES += ../../platform/mcu/lpc54102/CMSIS/Include
GLOBAL_INCLUDES += ../../platform/mcu/lpc54102/drivers
GLOBAL_INCLUDES += ../../platform/mcu/lpc54102/mcuxpresso
GLOBAL_INCLUDES += ../../platform/mcu/lpc54102/utilities
GLOBAL_INCLUDES += ../../platform/mcu/lpc54102/utilities/str
GLOBAL_INCLUDES += ../../platform/mcu/lpc54102/utilities/log
GLOBAL_INCLUDES += ../../platform/mcu/lpc54102/utilities/io



$(NAME)_SOURCES     :=
$(NAME)_SOURCES     += ./board.c
$(NAME)_SOURCES     += ./clock_config.c
$(NAME)_SOURCES     += ./pin_mux.c

