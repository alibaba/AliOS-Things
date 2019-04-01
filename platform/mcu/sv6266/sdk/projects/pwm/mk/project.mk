################################################################
# Project global
################################################################

FLASH_MODE        := 1
XIP_MODE          := 1
ROM_MODE          := 0
ROM_VERSION       := v0006
TCPIPSTACK_EN     := 1

# support DEBUG, RELEASE
BUILD_OPTION      := DEBUG
#BUILD_OPTION     := RELEASE

SUPPORT_LOW_POWER := 0

SUPPORT_BOOTLOADER					?= 1
SUPPORT_PARTITION_MP_TABLE			:= 1
SUPPORT_PARTITION_CFG_TABLE			:= 1
SUPPORT_PARTITION_USER_RAW    		:= 1
SETTING_THROUGHPUT_HIGH			    := 1
SETTING_UART_FW_UPGRADE 			:= 1

################################################################
# Project header
################################################################
# 0x00: reserved

# 0x04: xtal
#BOARD             := EVB_v1_26M
#BOARD             := EVB_v1_25M
#BOARD             := EVB_v1_38M
#BOARD             := EVB_v1_20M
#BOARD             := EVB_v1_24M
#BOARD             := EVB_v1_25M
#BOARD             := EVB_v1_19M
#BOARD             := EVB_v1_40M
#BOARD             := SSV6006_MP_S236_19M
#BOARD             := SSV6006_MP_S236_20M
#BOARD             := SSV6006_MP_S236_24M
BOARD             := SSV6006_MP_S236_25M
#BOARD             := SSV6006_MP_S236_26M
#BOARD             := SSV6006_MP_S236_38M
#BOARD             := SSV6006_MP_S236_40M
#BOARD             := SSV6006_MP_S236_52M
#BOARD             := SSV6006_MP_S197_25M
#BOARD             := SSV6006_MP_S197_40M
#BOARD             := SSV6006_SHUTTLE_S197_25M
#BOARD             := SSV6006_SHUTTLE_S197_40M
include     $(SDKDIR)/build/target_def.mk
include     $(SDKDIR)/build/xtal.mk

# 0x08: bus clock
SYS_BUS_CLK     := 80M
#SYS_BUS_CLK     := 40M

# 0x0c: xip bit
XIP_BIT			:= 4
GLOBAL_DEF		+= -DXIP_BIT=$(XIP_BIT)

# 0x10: partition main size
SETTING_PARTITION_MAIN_SIZE			:= 620K

# 0x14: flash total size
SETTING_FLASH_TOTAL_SIZE			:= 2M
GLOBAL_DEF		+= -DSETTING_FLASH_TOTAL_SIZE=$(SETTING_FLASH_TOTAL_SIZE)

# 0x18: psram heap base
SETTING_PSRAM_HEAP_BASE				:= 0
GLOBAL_DEF		+= -DSETTING_PSRAM_HEAP_BASE=$(SETTING_PSRAM_HEAP_BASE)

# 0x1c: psram heap size
SETTING_PSRAM_HEAP_SIZE				:= 0
GLOBAL_DEF		+= -DSETTING_PSRAM_HEAP_SIZE=$(SETTING_PSRAM_HEAP_SIZE)

################################################################
# Build System Detail setting
################################################################

BUILD_SHOW_ILM_INFO					?= 1
BUILD_SHOW_DLM_INFO					?= 1

################################################################
# Project Detail setting
################################################################

ifeq ($(strip $(XIP_MODE)), 1)
LDSCRIPT_S  = $(PROJ_DIR)/ld/flash.lds.S
else
LDSCRIPT_S  = $(PROJ_DIR)/ld/sram.lds.S
endif

ifeq ($(strip $(TARGET_CHIP)), turismo)
else
endif

CROSS_COMPILER  = nds32le-elf-

DBGFLAGS    = -g -ggdb

ifeq ($(strip $(BUILD_OPTION)), DEBUG)
OPTFLAGS    = -O1
else
OPTFLAGS    = -O2
endif

FLASH_CTRL 		:= 1

ifeq ($(strip $(TARGET_CHIP)), turismo)
ifeq ($(strip $(TARGET_DEF)), FPGAv3)
FLASH_CTRL 		:= 2
endif

ifeq ($(strip $(TARGET_DEF)), ASICv2)
FLASH_CTRL 		:= 2
endif
endif
##############################################################
# project source
##############################################################
IMPORT_DIR 		:=  $(PROJ_DIR)/src/app
IMPORT_DIR 		+=  $(PROJ_DIR)/src/cfg
ifeq ($(strip $(ROM_MODE)), 1)
else
IMPORT_DIR 		+=  $(PROJ_DIR)/src/rom_connector
IMPORT_DIR              += components/crypto
IMPORT_DIR              += components/net/tcpip/lwip-1.4.0
endif
IMPORT_DIR 		+=  $(PROJ_DIR)/src/cli

PROJECT_SRC  	:= $(PROJ_DIR)/src/libc_patch.c
PROJECT_OBJ		:= $(PROJ_DIR)/src/do_printf.o

###################################################################
# Import OS
###################################################################
ifeq ($(strip $(MCU_DEF)), ANDES_N10)
IMPORT_DIR 		+= $(PROJ_DIR)/src/boot/mcu/n10
else
	$(error please define mcu)
endif

IMPORT_DIR		+= components/bsp/soc/soc_init
IMPORT_DIR      += components/bsp/soc/lowpower

###################################################################
# Import OSAL
###################################################################
ifeq ($(strip $(OS)), freertos)
IMPORT_DIR      += components/osal/freertos
IMPORT_DIR      += components/osal/freertos/kernel
endif

IMPORT_DIR 		+=  components/osal

###################################################################
# Import FSAL
###################################################################
IMPORT_DIR 		+=  components/fsal/spiffs
IMPORT_DIR 		+=  components/fsal
INCLUDE += -I$(SDKDIR)/components/fsal/spiffs
INCLUDE += -I$(SDKDIR)/components/fsal

GLOBAL_DEF		+= -DFEATURE_SPIFFS=1 -DFEATURE_FFS=1

###################################################################
# Import Driver
###################################################################
IMPORT_DIR 		+=  components/sys

ifeq ($(strip $(ROM_MODE)), 1)
else
IMPORT_DIR		+=  components/drv/timer
endif

IMPORT_DIR 		+=  components/drv/common
IMPORT_DIR 		+=  components/drv/idmanage
IMPORT_DIR 		+=  components/drv/mbox
IMPORT_DIR 		+=  components/drv/hwmac
IMPORT_DIR 		+=  components/drv/phy
IMPORT_DIR 		+=  components/drv/security
IMPORT_DIR 		+=  components/softmac
IMPORT_DIR 		+=  components/iotapi
IMPORT_DIR 		+=  components/net/udhcp
IMPORT_DIR 		+=  components/drv/dma
IMPORT_DIR 		+=  components/drv/wdt
IMPORT_DIR 		+=  components/drv/pwm
IMPORT_DIR 		+=  components/drv/gpio
IMPORT_DIR 		+=  components/drv/pinmux
IMPORT_DIR 		+=  components/drv/i2cmst
IMPORT_DIR 		+=  components/drv/uart
IMPORT_DIR 		+=  components/drv/hsuart
IMPORT_DIR      	+=  components/drv/adc
IMPORT_DIR      	+=  components/drv/tmr

ifeq ($(strip $(TINY_LIBC)), 1)
IMPORT_DIR 		+=  components/third_party/rtos-comm/libc
else
#IMPORT_DIR 		+=  components/drv/terminal
endif

IMPORT_DIR 		+=  components/netstack_wrapper

###################################################################
# Import Application
###################################################################
WAC_EN            := 0
HTTPD_EN          := 0
CJSON_EN          := 1
HTTPC_EN          := 0
MQTT_EN           := 0
SSL_EN            := 0
MBED_EN           := 0
IPERF3_EN         := 1
SMARTCONFIG_EN    := 0
PING_EN     	  := 1

ifeq ($(strip $(WAC_EN)), 1)
IMPORT_DIR 		+=  components/third_party/mDNSResponder
IMPORT_DIR 		+=  components/third_party/WAC_POSIX_Server
######################
#debug for tim
######################
SUPPORT_USER_HEAP := 0 
endif
ifeq ($(strip $(HTTPD_EN)), 1)
IMPORT_DIR 		+=  components/third_party/httpd
endif
ifeq ($(strip $(CJSON_EN)), 1)
IMPORT_DIR 		+=  components/third_party/cJSON
endif
ifeq ($(strip $(HTTPC_EN)), 1)
IMPORT_DIR 		+=  components/third_party/httpclient
endif
ifeq ($(strip $(MQTT_EN)), 1)
IMPORT_DIR 		+=  components/third_party/mqtt
endif
ifeq ($(strip $(SSL_EN)), 1)
ifeq ($(strip $(MBED_EN)), 1)
IMPORT_DIR 		+=  components/third_party/mbedtls
else
IMPORT_DIR 		+=  components/third_party/matrixssl
endif
IMPORT_DIR 		+=  components/third_party/sntp
endif

ifeq ($(strip $(IPERF3_EN)), 1)
IMPORT_DIR 		+=  components/third_party/iperf3.0
else
IMPORT_DIR      += components/net/tcpip/iperf-2.0.5
endif
ifeq ($(strip $(SMARTCONFIG_EN)), 1)
IMPORT_DIR		+=	components/third_party/smart_config
endif
ifeq ($(strip $(PING_EN)), 1)
IMPORT_DIR		+=	components/third_party/ping
endif
##############################################################
# Build flags
##############################################################
ifeq ($(strip $(MCU_DEF)), ANDES_N10)
CFLAGS      :=  -mcpu=n10 -march=v3 \
                -mcmodel=large
ifeq ($(strip $(BUILD_OPTION)), DEBUG)
CFLAGS		+=  -fno-omit-frame-pointer \
                -mv3push \
                -mno-fp-as-gp
endif
AFLAGS      :=  -mcpu=n10 -march=v3 \
                -mcmodel=large
ifeq ($(strip $(BUILD_OPTION)), DEBUG)
AFLAGS		+=  -fno-omit-frame-pointer \
                -mv3push \
                -mno-fp-as-gp
endif
#CFLAGS += -ffixed-$r6 -ffixed-$r7
else
	$(error please define MCU_DEF)
endif

CFLAGS 		+=  -DMINIMAL_STACK_SIZE=256
AFLAGS 		+=  -DMINIMAL_STACK_SIZE=256

CFLAGS      +=  \
				-malign-functions -falign-functions=4 \
				-ffunction-sections -fdata-sections -fno-builtin \
				-Wno-attributes \
				--short-enums \
			    -Wall \
				-Wno-unused-function \
				-Wno-unused-variable \
				-Wno-unused-but-set-variable \
				-DLWIP_NOASSERT \
                -fno-delete-null-pointer-checks

ifeq ($(strip $(OS)), freertos)
CFLAGS += -DCONFIG_OS_FREERTOS
endif

CFLAGS 		+=  \
				-DCHIP_ID=$(CHIP_ID) \
				-D$(TARGET_DEF) \
				-DCONFIG_CACHE_SUPPORT \
				-UCONFIG_EXT_INTC
AFLAGS 		+=  \
				-DCHIP_ID=$(CHIP_ID) \
				-D$(TARGET_DEF) \
				-DCONFIG_CACHE_SUPPORT \
				-UCONFIG_EXT_INTC


###                -mno-fp-as-gp \

ifeq ($(strip $(ROM_MODE)), 1)
CFLAGS 		+= -DROM_MODE
AFLAGS 		+= -DROM_MODE
endif

ifeq ($(strip $(TINY_LIBC)), 1)
CFLAGS 		+= -DTINY_LIBC
endif
CFLAGS      += $(GLOBAL_DEF) $(DBGFLAGS) $(OPTFLAGS)
AFLAGS      += $(DBGFLAGS) $(OPTFLAGS)

INCLUDE += -I$(SDKDIR)/components/bsp/mcu/$(MCU_DEF)
INCLUDE += -I$(SDKDIR)/components/bsp/soc/$(CHIP_NAME)/$(TARGET_DEF)
INCLUDE += -I$(SDKDIR)/components/bsp/soc/$(CHIP_NAME)
INCLUDE += -I$(SDKDIR)/components/osal
INCLUDE += -I$(SDKDIR)/components/drv

INC 		+= $(INCLUDE)
ifeq ($(strip $(OS)), freertos)
INC 		+= -I$(SDKDIR)/components/osal/freertos/nds32
INC 		+= -I$(SDKDIR)/components/osal/freertos/kernel/Source/include
INC 		+= -I$(SDKDIR)/components/osal/freertos
endif
INC 		+= -I$(SDKDIR)/components/inc/rom
INC         += -I$(SDKDIR)/components/inc
INC         += -I$(SDKDIR)/components
INC         += -I$(PROJ_DIR)/src/inc
INC         += -I$(PROJ_DIR)/src/inc/custom

AFLAGS      += $(INCLUDE) $(GLOBAL_DEF) -D${TARGET_DEF}

ifeq ($(strip $(XIP_MODE)), 1)
CFLAGS      += -DXIP_MODE
AFLAGS      += -DXIP_MODE
endif

CFLAGS 		+= $(XTAL)

ifeq ($(strip $(SUPPORT_LOW_POWER)), 1)
CFLAGS 		+= -DFEATURE_RETENTION_BOOT
AFLAGS 		+= -DFEATURE_RETENTION_BOOT
endif

ifeq ($(strip $(SETTING_THROUGHPUT_HIGH)), 1)
CFLAGS 		+= -DSETTING_THROUGHPUT_HIGH
AFLAGS 		+= -DSETTING_THROUGHPUT_HIGH
endif

ifeq ($(strip $(SUPPORT_USER_HEAP)), 1)
CFLAGS 		+= -DFEATURE_USER_HEAP
AFLAGS 		+= -DFEATURE_USER_HEAP
endif

ifeq ($(strip $(SUPPORT_PSRAM_HEAP)), 1)
CFLAGS 		+= -DFEATURE_PSRAM_HEAP
AFLAGS 		+= -DFEATURE_PSRAM_HEAP
endif

ifeq ($(strip $(SUPPORT_BOOTLOADER)), 1)
CFLAGS		+= -DSUPPORT_BOOTLOADER
endif

ifeq ($(strip $(SUPPORT_PARTITION_MP_TABLE)), 1)
CFLAGS		+= -DSUPPORT_PARTITION_MP_TABLE
endif

ifeq ($(strip $(SUPPORT_PARTITION_CFG_TABLE)), 1)
ifneq ($(strip $(CJSON_EN)), 1)
$(error cfg table must support cjson)
endif
CFLAGS		+= -DSUPPORT_PARTITION_CFG_TABLE
endif

ifeq ($(strip $(SUPPORT_PARTITION_USER_RAW)), 1)
CFLAGS		+= -DSUPPORT_PARTITION_USER_RAW
CFLAGS 		+= -DSETTING_PARTITION_USER_RAW_SIZE=$(SETTING_PARTITION_USER_RAW_SIZE)
endif

CFLAGS 		+= -DSETTING_PARTITION_MAIN_SIZE=$(SETTING_PARTITION_MAIN_SIZE)


ifeq ($(strip $(FLASH_CTRL)), 2)
CFLAGS 		+= -DFLASH_CTL_v2
else
ifeq ($(strip $(FLASH_CTRL)), 1)
CFLAGS 		+= -DFLASH_CTL_v1
endif
endif

ifeq ($(strip $(FLASH_MODE)), 1)
CFLAGS		+= -DLDS_FLASH_SIZE=$(SETTING_FLASH_SIZE)
endif

ifeq ($(strip $(SYS_BUS_CLK)), 80M)
CFLAGS      += -DSYS_BUS_SPEED=80
AFLAGS 		+= -DSYS_BUS_SPEED=80
else
ifeq ($(strip $(SYS_BUS_CLK)), 40M)
CFLAGS      += -DSYS_BUS_SPEED=40
AFLAGS 		+= -DSYS_BUS_SPEED=40
endif
endif

##############################################################
# Build Application flags
##############################################################
INCLUDE += -I$(SDKDIR)/components/net/tcpip/lwip-1.4.0/ports/icomm/include
INCLUDE += -I$(SDKDIR)/components/net/tcpip/lwip-1.4.0/src/include
INCLUDE += -I$(SDKDIR)/components/net/tcpip/lwip-1.4.0/src/include/ipv4
INCLUDE += -I$(SDKDIR)/components/third_party/mDNSResponder/inc
INCLUDE += -I$(SDKDIR)/components/third_party/WAC_POSIX_Server/Platform
INCLUDE += -I$(SDKDIR)/components/third_party/WAC_POSIX_Server/Sources
INCLUDE += -I$(SDKDIR)/components/third_party/WAC_POSIX_Server/Support
INCLUDE += -I$(SDKDIR)/components/third_party/matrixssl
INCLUDE += -I$(SDKDIR)/components/inc/crypto
INCLUDE += -I$(SDKDIR)/components/softmac
INCLUDE += -I$(SDKDIR)/components/iotapi
INCLUDE += -I$(SDKDIR)/components/netstack_wrapper

ifeq ($(strip $(WAC_EN)), 1)
CFLAGS 		+= -DWAC_EN
AFLAGS 		+= -DWAC_EN
endif

ifeq ($(strip $(SSL_EN)), 1)
CFLAGS 		+= -DSSL_EN
ifeq ($(strip $(MBED_EN)), 1)
CFLAGS 		+= -DMBED_EN
MBED_TURN           := 1
ifeq ($(strip $(MBED_TURN)), 1)
CFLAGS 		+= -DMBED_TURN
CFLAGS 		+= -DMBED_RO_TURN #1452Bytes
CFLAGS 		+= -DSHA512_TEXT_TURN #5040Bytes
CFLAGS 		+= -DAES_TEXT_TURN #2700Bytes
CFLAGS 		+= -DRSA_TEXT_TURN #152Bytes
CFLAGS 		+= -DBIGNUM_TEXT_TURN #2536Bytes
endif
endif
endif

ifeq ($(strip $(IPERF3_EN)), 1)
CFLAGS 		+= -DIPERF3_EN
AFLAGS 		+= -DIPERF3_EN
endif

ifeq ($(strip $(SMARTCONFIG_EN)), 1)
CFLAGS 		+= -DSMARTCONFIG_EN
AFLAGS 		+= -DSMARTCONFIG_EN
endif

ifeq ($(strip $(PING_EN)), 1)
CFLAGS 		+= -DPING_EN
AFLAGS 		+= -DPING_EN
endif

ifeq ($(strip $(TCPIPSTACK_EN)), 1)
CFLAGS 		+= -DTCPIPSTACK_EN
AFLAGS 		+= -DTCPIPSTACK_EN
endif

LDFLAGS 	:=
LDFLAGS     += -Wl,--gc-sections
###LDFLAGS     += -Wl,--mno-fp-as-gp
LDFLAGS     += -nostartfiles -Xlinker -M
LDFLAGS     += --specs=nosys.specs -Werror -mcmodel=large -g
#ANDES#

CFLAGS 		+= -DSETTING_UART_FW_UPGRADE=$(SETTING_UART_FW_UPGRADE)
AFLAGS 		+= -DSETTING_UART_FW_UPGRADE=$(SETTING_UART_FW_UPGRADE)

VERBOSE  ?= 0

