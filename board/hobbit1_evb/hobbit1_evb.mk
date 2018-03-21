NAME := board_hobbit1_2

JTAG := jlink

$(NAME)_TYPE := kernel
MODULE               := HOBBIT1_2
HOST_CHIP            := hobbit1_2
HOST_ARCH            := ck802
HOST_MCU_FAMILY      := csky
SUPPORT_BINS         := no

#$(NAME)_COMPONENTS  :=  tfs
#CONFIG_LIB_TFS := y
#CONFIG_TFS_ID2_RSA := y
#CONFIG_TFS_ID2_3DES := n
#CONFIG_TFS_EMULATE := n
#CONFIG_TFS_ON_LINE := n
#CONFIG_TFS_OPENSSL := n
#CONFIG_TFS_MBEDTLS := n
#CONFIG_TFS_ALICRYPTO := n
#CONFIG_TFS_DEBUG := n
#CONFIG_TFS_TEE := n
#CONFIG_TFS_SW := y
#CONFIG_TFS_TEST := n

$(NAME)_SOURCES := board_init.c
$(NAME)_SOURCES += net/ethernetif.c

GLOBAL_INCLUDES += include/
GLOBAL_DEFINES += STDIO_UART=0 MBEDTLS_AES_ROM_TABLES=1

define get-os-version
"AOS-R"-$(CURRENT_TIME)
endef

CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)
CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_CKHOBBIT
CONFIG_SYSINFO_DEVICE_NAME := CKHOBBIT

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -std=gnu99
#GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)/board/ckhobbit1_2
GLOBAL_LDFLAGS  += -Wl,-ckmap='ckhobbit1_2.map' -lm

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk
