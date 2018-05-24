
NAME := board_lpcxpresso54608

MODULE              := 1062
HOST_ARCH           := Cortex-M4
HOST_MCU_FAMILY     := lpc54608

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_LPC54608
CONFIG_SYSINFO_DEVICE_NAME := LPC54608
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DCPU_LPC54608J512ET180
GLOBAL_CFLAGS += -D__USE_CMSIS -D__MULTICORE_MASTER
GLOBAL_CFLAGS += -D__NEWLIB__



GLOBAL_LDFLAGS += 

GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += ../../platform/mcu/lpc54608/
GLOBAL_INCLUDES += ../../platform/mcu/lpc54608/CMSIS/Include
GLOBAL_INCLUDES += ../../platform/mcu/lpc54608/drivers
GLOBAL_INCLUDES += ../../platform/mcu/lpc54608/gcc
GLOBAL_INCLUDES += ../../platform/mcu/lpc54608/utilities
GLOBAL_INCLUDES += ../../platform/mcu/lpc54608/utilities/str
GLOBAL_INCLUDES += ../../platform/mcu/lpc54608/utilities/log
GLOBAL_INCLUDES += ../../platform/mcu/lpc54608/utilities/io



$(NAME)_SOURCES     :=
$(NAME)_SOURCES     += ./board.c
$(NAME)_SOURCES     += ./clock_config.c
$(NAME)_SOURCES     += ./pin_mux.c
$(NAME)_SOURCES     += ./fsl_phy.c

