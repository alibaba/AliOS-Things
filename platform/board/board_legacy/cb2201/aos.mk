NAME := board_cb2201

JTAG := jlink

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := configuration for board cb2201
MODULE             := CH2201
HOST_CHIP          := ch2201
HOST_ARCH          := ck802
HOST_MCU_FAMILY    := mcu_csky
SUPPORT_MBINS      := no

# Link Security Config
CONFIG_LS_DEBUG      := n
CONFIG_LS_ID2_OTP    := y
CONFIG_LS_KM_SE      := n
CONFIG_LS_KM_TEE     := y

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

$(NAME)_SOURCES := board_init.c

ifeq ($(LWIP),1)
$(NAME)_SOURCES += net/ethernetif.c
endif

GLOBAL_INCLUDES += include/
GLOBAL_DEFINES  += STDIO_UART=0 MBEDTLS_AES_ROM_TABLES=1
GLOBAL_DEFINES  += CLI_CONFIG_SUPPORT_BOARD_CMD=1
GLOBAL_DEFINES  += VFS_CONFIG_STAT_INCLUDE_SIZE=0


CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_CKHOBBIT
CONFIG_SYSINFO_DEVICE_NAME   := CKHOBBIT

GLOBAL_CFLAGS  += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS  += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS  += -std=gnu99
GLOBAL_LDFLAGS += -Wl,-ckmap='cb2201.map' -Wl,-zmax-page-size=1024 -lm

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)

