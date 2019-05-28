#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#

NAME := sv6266
SDKDIR := sdk

HOST_OPENOCD := ICEman

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += rhino hal netmgr framework.common cli cjson digest_algorithm alicrypto
$(NAME)_COMPONENTS += protocols.net
$(NAME)_COMPONENTS += libc

$(NAME)_COMPONENTS += platform/mcu/sv6266/$(SDKDIR)/components/bsp/soc/soc_init
$(NAME)_COMPONENTS += platform/mcu/sv6266/osal
$(NAME)_COMPONENTS += platform/mcu/sv6266/cfg

$(NAME)_PREBUILT_LIBRARY := lib/bootloader.a
$(NAME)_PREBUILT_LIBRARY += lib/rf_api.a
#$(NAME)_PREBUILT_LIBRARY += lib/cfg.a
$(NAME)_PREBUILT_LIBRARY += lib/crypto.a
#$(NAME)_PREBUILT_LIBRARY += lib/ota.a
$(NAME)_PREBUILT_LIBRARY += lib/n10.a
#$(NAME)_PREBUILT_LIBRARY += lib/soc_init.a
#$(NAME)_PREBUILT_LIBRARY += lib/osal.a
$(NAME)_PREBUILT_LIBRARY += lib/sys.a
$(NAME)_PREBUILT_LIBRARY += lib/timer.a
$(NAME)_PREBUILT_LIBRARY += lib/common.a
$(NAME)_PREBUILT_LIBRARY += lib/idmanage.a
$(NAME)_PREBUILT_LIBRARY += lib/mbox.a
$(NAME)_PREBUILT_LIBRARY += lib/hwmac.a
$(NAME)_PREBUILT_LIBRARY += lib/phy.a
$(NAME)_PREBUILT_LIBRARY += lib/security.a
$(NAME)_PREBUILT_LIBRARY += lib/softmac.a
$(NAME)_PREBUILT_LIBRARY += lib/iotapi.a
$(NAME)_PREBUILT_LIBRARY += lib/udhcp.a
$(NAME)_PREBUILT_LIBRARY += lib/wdt.a
$(NAME)_PREBUILT_LIBRARY += lib/pinmux.a
$(NAME)_PREBUILT_LIBRARY += lib/netstack_wrapper.a
$(NAME)_PREBUILT_LIBRARY += lib/ali_port.a
$(NAME)_PREBUILT_LIBRARY += lib/efuseapi.a
$(NAME)_PREBUILT_LIBRARY += lib/efuse.a
$(NAME)_PREBUILT_LIBRARY += lib/tmr.a
$(NAME)_PREBUILT_LIBRARY += lib/gpio.a
$(NAME)_PREBUILT_LIBRARY += lib/drv_uart.a
$(NAME)_PREBUILT_LIBRARY += lib/lowpower.a
$(NAME)_PREBUILT_LIBRARY += lib/pwm.a
$(NAME)_PREBUILT_LIBRARY += lib/adc.a


GLOBAL_INCLUDES += port

GLOBAL_INCLUDES += $(SDKDIR)/components/bsp/mcu/ANDES_N10
GLOBAL_INCLUDES += $(SDKDIR)/components/bsp/soc/ssv6006/ASICv2
GLOBAL_INCLUDES += $(SDKDIR)/components/bsp/soc/ssv6006
GLOBAL_INCLUDES += $(SDKDIR)/components/osal
GLOBAL_INCLUDES += $(SDKDIR)/components/inc
GLOBAL_INCLUDES += $(SDKDIR)/components
GLOBAL_INCLUDES += $(SDKDIR)/components/tools/atcmd
GLOBAL_INCLUDES += $(SDKDIR)/components/inc/crypto \
                   $(SDKDIR)/components/softmac \
                   $(SDKDIR)/components/iotapi \
                   $(SDKDIR)/components/netstack_wrapper \
				   ../../../kernel/protocols/net/include \
				   ../../../kernel/protocols/net/port/include \
				   inc
GLOBAL_INCLUDES += ../../../tools/cli/
GLOBAL_INCLUDES += $(SDKDIR)/components/drv/rf/
GLOBAL_INCLUDES += cfg
GLOBAL_INCLUDES += inc/custom
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

# 0x0c: xip bit
XIP_BIT := 4
GLOBAL_DEFINES += XIP_BIT=$(XIP_BIT)

# 0x10: partition main size
SETTING_PARTITION_MAIN_SIZE := 820K
GLOBAL_DEFINES += SETTING_PARTITION_MAIN_SIZE=$(SETTING_PARTITION_MAIN_SIZE)

# 0x14: flash total size
SETTING_FLASH_TOTAL_SIZE := 2M
GLOBAL_DEFINES += SETTING_FLASH_TOTAL_SIZE=$(SETTING_FLASH_TOTAL_SIZE)

# 0x18: psram heap base, not supported
SETTING_PSRAM_HEAP_BASE := 0
GLOBAL_DEFINES += SETTING_PSRAM_HEAP_BASE=$(SETTING_PSRAM_HEAP_BASE)

# 0x1c: psram heap size, not supported
SETTING_PSRAM_HEAP_SIZE := 0
GLOBAL_DEFINES += SETTING_PSRAM_HEAP_SIZE=$(SETTING_PSRAM_HEAP_SIZE)

################################################################
# kv
################################################################
#GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=5
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192

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

#GLOBAL_LDS_FILES += platform/mcu/sv6266/$(SDKDIR)/projects/mac_atcmd/ld/flash.lds.S
GLOBAL_LDS_FILES += platform/mcu/sv6266/ld/flash.lds.S
GLOBAL_LDFLAGS += -nostartfiles --specs=nosys.specs -mcmodel=large
GLOBAL_LDFLAGS += platform/mcu/sv6266/do_printf.o

$(NAME)_INCLUDES := $(SDKDIR)/components/drv
$(NAME)_SOURCES :=	aos.c \
					libc_patch.c \
					port/soc_impl.c \
					port/port_tick.c \
					hal/uart.c \
					hal/flash_port.c \
                    hal/wifi_port.c \
                    hal/rf_cmd.c \
                    hal/pwm.c \
                    hal/adc.c \
		    hal/gpio.c \
		    hal/i2c.c  \
		    hal/spi.c  \
                    $(SDKDIR)/components/net/tcpip/lwip-1.4.0/src/netif/ethernetif.c \
					hal/hw.c
