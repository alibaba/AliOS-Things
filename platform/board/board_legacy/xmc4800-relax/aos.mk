NAME := board_xmc4800-relax

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board xmc4800-relax
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_xmc
SUPPORT_MBINS      := no
HOST_MCU_NAME      := xmc4800
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init network
$(NAME)_COMPONENTS-$(BSP_SUPPORT_EXTERNAL_MODULE) += external_module netmgr

$(NAME)_SOURCES += config/partition_conf.c \
                   config/k_config.c      \
                   startup/board.c      \
                   startup/startup.c      \

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup/startup_XMC4800_armcc.S
$(NAME)_LINK_FILES := startup_XMC4800_armcc.o
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES    += startup/startup_XMC4800_icc.S
else
$(NAME)_SOURCES    += startup/startup_XMC4800_gcc.S
endif

GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=32768 #32kb
GLOBAL_DEFINES += KV_CONFIG_BLOCK_SIZE_BITS=14 #(1 << 14) = 16kb

GLOBAL_INCLUDES += config/    \
                   drivers/   \
                   drivers/hal/ \

GLOBAL_CFLAGS += -DXMC4800_F144x2048 -DCENTRALIZE_MAPPING

ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=platform/board/board_legacy/xmc4800-relax/XMC4800x2048.sct
else ifeq ($(COMPILER),iar)
GLOBAL_LDFLAGS += --config platform/board/board_legacy/xmc4800-relax/XMC4800x2048.icf
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/xmc4800-relax/XMC4800x2048.ld
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_xmc4800-relax
CONFIG_SYSINFO_DEVICE_NAME   := xmc4800-relax

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

# Keil project support
$(NAME)_KEIL_VENDOR = Infineon
$(NAME)_KEIL_DEVICE = XMC4800-F144x2048
# Iar project support: OGChipSelectEditMenu
$(NAME)_IAR_OGCMENU = XMC4800-F144x2048   Infineon XMC4800-F144x2048