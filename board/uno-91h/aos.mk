NAME := board_uno-91h

JTAG := jlink

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board uno-91h
MODULE             := xxx
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_RDA5981x

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES := board.c \
                   startup_uno-91h.s

GLOBAL_INCLUDES += .
GLOBAL_DEFINES  += STDIO_UART=0
GLOBAL_DEFINES  += RHINO_CONFIG_WORKQUEUE=0

ifeq ($(shell uname -o), Msys)
CURRENT_TIME = $(shell ${DATE} +%Y%m%d.%H%M)
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_RDA5981
CONFIG_SYSINFO_DEVICE_NAME   := UNO-91H

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_APP_VERSION=\"$(CONFIG_SYSINFO_APP_VERSION)\"

GLOBAL_LDFLAGS  += -L $($(NAME)_LOCATION)

ifeq ($(APP), linkkitapp)
GLOBAL_LDFLAGS += board/uno-91h/hfilop/hfilop.a
else ifeq ($(APP), inkkit_gateway)
GLOBAL_LDFLAGS += board/uno-91h/hfilop/hfilop.a
else
GLOBAL_DEFINES += DELETE_HFILOP_CODE
endif

# Global defines
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -DRDA5981X -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
GLOBAL_CFLAGS  += -DRDA5981A

GLOBAL_LDFLAGS += -T uno-91h.ld

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=
