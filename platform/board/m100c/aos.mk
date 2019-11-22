NAME := board_m100c

JTAG := jlink_swd

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board m100c

MODULE          := 1062
HOST_ARCH       := Cortex-M3
HOST_MCU_FAMILY := mcu_efm32gxx
SUPPORT_MBINS   := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

LORACHIP           := sx1276

GLOBAL_DEFINES += RHINO_CONFIG_WORKQUEUE=0 DEBUG_CONFIG_ERRDUMP=0
GLOBAL_DEFINES += EFM32G222F128

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_M100C
CONFIG_SYSINFO_DEVICE_NAME   := M100C

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS += -L $($(NAME)_LOCATION)

GLOBAL_INCLUDES += .
GLOBAL_INCLUDES += ./inc

$(NAME)_SOURCES := src/board.c
$(NAME)_SOURCES += src/hw.c
$(NAME)_SOURCES += src/debug.c
$(NAME)_SOURCES += src/spi-board.c
$(NAME)_SOURCES += src/rtc-board.c
$(NAME)_SOURCES += src/gpio-board.c
$(NAME)_SOURCES += src/delay-board.c
$(NAME)_SOURCES += src/gpio-board.c
$(NAME)_SOURCES += src/lorawan_port.c
$(NAME)_SOURCES += src/sx1276-board.c
$(NAME)_SOURCES += src/utilities.c
$(NAME)_SOURCES += src/low_power.c

