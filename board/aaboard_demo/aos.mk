NAME := board_aaboard_demo

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 0.0.1
$(NAME)_SUMMARY    := configuration for board aaboard_demo
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := aamcu_demo
SUPPORT_MBINS       := no
HOST_MCU_NAME      := aamcu1_demo
ENABLE_VFP         := 1

GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=32768 #32kb
GLOBAL_DEFINES += CONFIG_AOS_KV_BLK_BITS=14 #(1 << 14) = 16kb
GLOBAL_CFLAGS  += -DSTM32F429xx -DCENTRALIZE_MAPPING

$(NAME)_SOURCES += config/k_config.c \
                   config/partition_conf.c  \
                   drivers/msp_conf.c    \
                   startup/board.c   \
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

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/  \