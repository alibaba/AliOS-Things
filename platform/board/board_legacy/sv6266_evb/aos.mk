NAME := board_sv6266_evb

JTAG := ICEman

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := configuration for board sv6266_evb
MODULE             := 1062
HOST_ARCH          := ANDES_N10
HOST_MCU_FAMILY    := mcu_sv6266
SUPPORT_MBINS      := no
ENABLE_VFP         := 0

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) newlib_stub kernel_init netmgr cjson cli

################################################################
# kv
################################################################
#GLOBAL_DEFINES += KV_CONFIG_PARTITION=5
GLOBAL_DEFINES += KV_CONFIG_TOTAL_SIZE=8192
GLOBAL_DEFINES += SV6266

GLOBAL_INCLUDES += . \
                   config/ \
                   startup/

GLOBAL_LDS_FILES += platform/board/board_legacy/sv6266_evb/flash.lds.S

# Global defines

$(NAME)_SOURCES += config/k_config.c \
                   config/partition_conf.c \
                   config/port_tick.c \
                   startup/board.c \
                   startup/startup.c

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_SV6266_EVB
CONFIG_SYSINFO_DEVICE_NAME := SV6266_EVB

GLOBAL_CFLAGS += -DAOS_OTA_BANK_SINGLE
GLOBAL_CFLAGS += -USYSINFO_OS_VERSION -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -USYSINFO_PRODUCT_MODEL -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -USYSINFO_DEVICE_NAME -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

