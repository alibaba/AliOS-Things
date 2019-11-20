NAME := board_aaboard_demo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board aaboard_demo
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_aamcu_demo
SUPPORT_MBINS       := no
HOST_MCU_NAME      := aamcu1_demo
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init

GLOBAL_CFLAGS  += -DSTM32F429xx -DCENTRALIZE_MAPPING

$(NAME)_SOURCES += config/k_config.c       \
                   config/partition_conf.c \
                   drivers/msp_conf.c      \
                   startup/board.c         \
                   startup/startup.c

ifeq ($(COMPILER), armcc)
$(NAME)_SOURCES    += startup/startup_keil.s
$(NAME)_LINK_FILES := startup/startup_keil.o
GLOBAL_LDFLAGS += -L --scatter=board/aaboard_demo/aaboard_demo.sct
else ifeq ($(COMPILER), iar)
$(NAME)_SOURCES    += startup/startup_iar.s
GLOBAL_LDFLAGS += --config board/aaboard_demo/aaboard_demo.icf
else
$(NAME)_SOURCES    += startup/startup_gcc.s
GLOBAL_LDFLAGS += -T board/aaboard_demo/aaboard_demo.ld
endif

GLOBAL_INCLUDES += .        \
                   config/  \
                   drivers/ \
                   startup/

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_aaboard_demo
CONFIG_SYSINFO_DEVICE_NAME   := aaboard_demo

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
