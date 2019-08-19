NAME := board_dh5021a_evb

JTAG := jlink

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board dh5021a_evb
MODULE             := DH5021A
HOST_CHIP          := dh5021a
HOST_ARCH          := ck803
HOST_MCU_FAMILY    := mcu_dahua
SUPPORT_MBINS      := no

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES := board_init.c

ifeq ($(LWIP),1)
$(NAME)_SOURCES += net/ethernetif.c
endif

GLOBAL_INCLUDES += include/
# net/lwip/port/include
GLOBAL_DEFINES += STDIO_UART=0 MBEDTLS_AES_ROM_TABLES=1
GLOBAL_DEFINES += CLI_CONFIG_SUPPORT_BOARD_CMD=1

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_DH5021A
CONFIG_SYSINFO_DEVICE_NAME   := DH5021A

GLOBAL_CFLAGS  += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS  += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS  += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS  += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"
GLOBAL_CFLAGS  += -DSYSINFO_BOARD=\"$(MODULE)\"
GLOBAL_CFLAGS  += -std=gnu99
GLOBAL_LDFLAGS += -Wl,-ckmap='dh5021a.map' -Wl,-zmax-page-size=1024 -lm

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/gen_crc_bin.mk

