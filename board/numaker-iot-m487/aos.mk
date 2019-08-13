NAME := board_numaker-iot-m487

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := configuration for board numaker-iot-m487
MODULE             := 1062
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_m487jidae
SUPPORT_MBINS      := no
HOST_MCU_NAME      := M487JIDAE
ENABLE_VFP         := 1

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr

GLOBAL_DEFINES += NUMAKER_IOT_M487=1 STDIO_UART=0 STDIO_UART_BUADRATE=115200
GLOBAL_DEFINES += CLI_CONFIG_SUPPORT_BOARD_CMD=1
GLOBAL_DEFINES += KV_CONFIG_PARTITION=5

$(NAME)_SOURCES += config/k_config.c \
                   config/partition_conf.c \
                   startup/board_cli.c \
                   startup/board.c   \
                   startup/startup.c

$(NAME)_SOURCES += startup/startup_M480_gcc.c


ifeq ($(COMPILER),armcc)
GLOBAL_LDFLAGS += -L --scatter=board/numaker-iot-m487/M487.sct
else
GLOBAL_LDFLAGS  += -T board/numaker-iot-m487/M487.ld
endif

GLOBAL_INCLUDES += .    \
                   config/   \
                   drivers/  \
                   startup/

ETHERNET := 1
#WIFI  := 0

ifeq ($(ETHERNET),1)
$(NAME)_SOURCES += drivers/ethernetif.c
$(NAME)_COMPONENTS  += lwip yloop
no_with_lwip := 0
press_test := 1
HW_CRYPTO_AES_NUVOTON := 1
GLOBAL_DEFINES += WITH_LWIP
GLOBAL_DEFINES += LWIP_MAILBOX_QUEUE
GLOBAL_DEFINES += LWIP_TIMEVAL_PRIVATE=0
else ifeq ($(WIFI),1)
SAL := 0
press_test := 1
no_with_lwip := 1
$(NAME)_COMPONENTS  += sal device_sal_mk3060
GLOBAL_DEFINES += WITH_SAL
GLOBAL_DEFINES += DEV_SAL_MK3060
else
GLOBAL_DEFINES += CONFIG_NO_TCPIP
endif


ifeq ($(COMPILER),armcc)
$(NAME)_LINK_FILES := startup/startup_M480_gcc.o
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_NUMAKER_M487
CONFIG_SYSINFO_DEVICE_NAME := $(NAME)
CONFIG_SYSINFO_OS_VERSION := 100

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"$(HOST_ARCH)\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"

ifeq ($(HOST_OS), Win32)
EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/gen_crc_bin.mk
endif


