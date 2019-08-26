NAME := mcu_sv6266

HOST_OPENOCD := ICEman

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu sv6266

$(NAME)_COMPONENTS += rhino

SDKDIR := drivers/sdk

$(NAME)_PREBUILT_LIBRARY := drivers/lib/bootloader.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/rf_api.a
#$(NAME)_PREBUILT_LIBRARY += lib/cfg.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/crypto.a
#$(NAME)_PREBUILT_LIBRARY += lib/ota.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/n10.a
#$(NAME)_PREBUILT_LIBRARY += lib/soc_init.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/osal.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/sys.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/timer.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/common.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/idmanage.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/mbox.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/hwmac.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/phy.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/security.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/softmac.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/iotapi.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/udhcp.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/wdt.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/pinmux.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/netstack_wrapper.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/ali_port.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/efuseapi.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/efuse.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/tmr.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/gpio.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/drv_uart.a
$(NAME)_PREBUILT_LIBRARY += drivers/lib/lowpower.a

GLOBAL_INCLUDES += drivers/inc

GLOBAL_INCLUDES += $(SDKDIR)/components/bsp/mcu/ANDES_N10
GLOBAL_INCLUDES += $(SDKDIR)/components/bsp/soc/ssv6006/ASICv2
GLOBAL_INCLUDES += $(SDKDIR)/components/bsp/soc/ssv6006
GLOBAL_INCLUDES += $(SDKDIR)/components/osal
GLOBAL_INCLUDES += $(SDKDIR)/components/inc
GLOBAL_INCLUDES += $(SDKDIR)/components
GLOBAL_INCLUDES += $(SDKDIR)/projects/mac_atcmd/src/inc
GLOBAL_INCLUDES += $(SDKDIR)/projects/mac_atcmd/src/inc/custom
GLOBAL_INCLUDES += $(SDKDIR)/components/tools/atcmd
GLOBAL_INCLUDES += $(SDKDIR)/components/inc/crypto
GLOBAL_INCLUDES += $(SDKDIR)/components/softmac
GLOBAL_INCLUDES += $(SDKDIR)/components/iotapi
GLOBAL_INCLUDES += $(SDKDIR)/components/netstack_wrapper
GLOBAL_INCLUDES += $(SDKDIR)/components/drv/rf
GLOBAL_INCLUDES += $(SDKDIR)/projects/mac_atcmd/src/cfg
GLOBAL_INCLUDES += $(SDKDIR)/components/drv

################################################################
# Project header
################################################################
XIP_MODE := 1
ifeq ($(strip $(XIP_MODE)), 1)
GLOBAL_DEFINES += XIP_MODE
endif

ROM_MODE := 0
ifeq ($(strip $(ROM_MODE)), 1)
GLOBAL_DEFINES += ROM_MODE
else
GLOBAL_DEFINES += NO_ROM
endif

GLOBAL_DEFINES += ASICv2
GLOBAL_DEFINES += CONFIG_CACHE_SUPPORT
GLOBAL_DEFINES += CONFIG_ENABLE_WDT

SUPPORT_LOW_POWER := 0
ifeq ($(strip $(SUPPORT_LOW_POWER)), 1)
GLOBAL_DEFINES += FEATURE_RETENTION_BOOT
endif

GLOBAL_DEFINES += SUPPORT_PARTITION_MP_TABLE
GLOBAL_DEFINES += SUPPORT_PARTITION_CFG_TABLE
GLOBAL_DEFINES += SUPPORT_PARTITION_USER_RAW

SETTING_THROUGHPUT_HIGH := 0
ifeq ($(strip $(SETTING_THROUGHPUT_HIGH)), 1)
GLOBAL_DEFINES += SETTING_THROUGHPUT_HIGH
endif

GLOBAL_DEFINES += FLASH_CTL_v2

GLOBAL_DEFINES += CONFIG_OS_RHINO
# 0x00: reserved

# 0x04: xtal
XTAL := 40
GLOBAL_DEFINES += XTAL=$(XTAL)

# 0x08: bus clock
SYS_BUS_CLK := 80M
ifeq ($(strip $(SYS_BUS_CLK)), 80M)
GLOBAL_DEFINES += SYS_BUS_SPEED=80
endif
ifeq ($(strip $(SYS_BUS_CLK)), 40M)
GLOBAL_DEFINES += SYS_BUS_SPEED=40
endif
$(info $(GLOBAL_DEFINES))

# 0x0c: xip bit
XIP_BIT := 4
GLOBAL_DEFINES += XIP_BIT=$(XIP_BIT)

# 0x10: partition main size
SETTING_PARTITION_MAIN_SIZE := 820K
GLOBAL_DEFINES += SETTING_PARTITION_MAIN_SIZE=$(SETTING_PARTITION_MAIN_SIZE)

# 0x14: flash total size
SETTING_FLASH_TOTAL_SIZE := 2M
GLOBAL_DEFINES += SETTING_FLASH_TOTAL_SIZE=$(SETTING_FLASH_TOTAL_SIZE)

# 0x18: psram heap base
SETTING_PSRAM_HEAP_BASE := 0
GLOBAL_DEFINES += SETTING_PSRAM_HEAP_BASE=$(SETTING_PSRAM_HEAP_BASE)

# 0x1c: psram heap size
SETTING_PSRAM_HEAP_SIZE := 0
GLOBAL_DEFINES += SETTING_PSRAM_HEAP_SIZE=$(SETTING_PSRAM_HEAP_SIZE)

################################################################
# build
################################################################

GLOBAL_CFLAGS += -std=gnu99 -fgnu89-inline -Wno-format
GLOBAL_CFLAGS += -mcpu=n10 -march=v3 -mcmodel=large
GLOBAL_CFLAGS += -DPORTING_DEBUG
#GLOBAL_CFLAGS += -fno-omit-frame-pointer -mv3push -mno-fp-as-gp
GLOBAL_CFLAGS += -DMINIMAL_STACK_SIZE=128
GLOBAL_CFLAGS += -DTCPIPSTACK_EN
GLOBAL_CFLAGS += \
                -malign-functions -falign-functions=4 \
                -ffunction-sections -fdata-sections -fno-builtin \
                -Wno-attributes \
                --short-enums \
                -w -Wall \
                -Wno-unused-function \
                -Wno-unused-variable \
                -Wno-unused-but-set-variable \
                -DLWIP_NOASSERT \
                -fno-delete-null-pointer-checks

#GLOBAL_LDS_FILES += platform/mcu/sv6266/ld/flash.lds.S
GLOBAL_LDFLAGS += -nostartfiles --specs=nosys.specs -mcmodel=large
GLOBAL_LDFLAGS += platform/mcu/sv6266/drivers/do_printf.o

$(NAME)_INCLUDES += $(SDKDIR)/components/third_party/cJSON \
                    $(SDKDIR)/components/sys \

$(NAME)_SOURCES  += drivers/libc_patch.c \
                    drivers/osal_rhino.c \
                    drivers/cfg/sa_cfg.c \
                    drivers/cfg/mac_cfg.c \
                    drivers/cfg/user_cfg.c \
                    hal/uart.c \
                    hal/flash_port.c \
                    hal/wifi_port.c \
                    hal/rf_cmd.c \
                    hal/hw.c \
                    hal/wdg.c \
                    $(SDKDIR)/components/net/tcpip/lwip-1.4.0/src/netif/ethernetif.c \
                    $(SDKDIR)/components/bsp/soc/soc_init/init_mem.c \
                    $(SDKDIR)/components/bsp/soc/soc_init/soc_init.c

EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk

