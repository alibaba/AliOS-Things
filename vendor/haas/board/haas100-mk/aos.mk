NAME := board_haas100-mk

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board haas100
MODULE             := haas100
HOST_ARCH          := Cortex-M33
HOST_MCU_FAMILY    := mcu_haas1000
HOST_MCU_NAME      := haas1000
SUPPORT_MBINS      := yes
ENABLE_USPACE      := 1
#ENABLE_VFP        := 0
APP_FORMAT         := bin
export SECURE_OTA_BOOT    := yes

#use script to make littlefs bin
#export MAKE_DATA_IMAGE	:= yes

# MBINS will be deprecated, please use AOS_KERNEL_MODE
ifeq ($(AOS_KERNEL_MODE),y)

ifneq ($(AOS_2NDBOOT_SUPPORT),yes)

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init arch_armv8m-mk vfs irq
$(NAME)_COMPONENTS += v_partition_drv
$(NAME)_COMPONENTS += v_gpio_drv v_uart_drv v_flash_drv ota_adapt

ifeq ($(CONFIG_WORK_WITH_WIFI),y)
GLOBAL_DEFINES += WITH_LWIP
$(NAME)_COMPONENTS-$(CONFIG_WORK_WITH_WIFI) += lwip netmgr
endif
ifeq ($(CONFIG_WORK_WITH_ETH),y)
$(NAME)_COMPONENTS-$(CONFIG_WORK_WITH_ETH) += device_sal_ch395
endif
ifeq ($(CONFIG_WORK_WITH_LTE),y)
$(NAME)_COMPONENTS-$(CONFIG_WORK_WITH_LTE) += device_sal_ec20
endif


$(NAME)_SOURCES += config/k_config.c \
                   config/board.c \
                   startup/startup.c \
                   drivers/led.c \
                   drivers/key.c \
                   drivers/watchdog.c \
                   drivers/ch395_spi.c \
                   drivers/di.c \
                   drivers/do.c \
                   drivers/exp_adc.c

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
ifeq ($(SECURE_OTA_BOOT), yes)
ifneq ($(MBINS), app)
EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/release/auto_build_tool/haas1000_genbin.mk
endif
endif

else
GLOBAL_DEFINES += AOS_2ND_BOOT_NO_LDS AOS_2ND_BOOT_AB AOS_2ND_BOOT_LATER_ERASE
$(NAME)_SOURCES += config/partition_conf.c

$(NAME)_COMPONENTS += ota_2ndboot
EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/release/auto_build_tool/haas1000_2ndboot_genbin.mk
endif

else

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) arch_armv8m-mk vfs

endif

MACROS := -DCHIP_HAAS1000 -DCHIP_HAS_UART=3 -DRESAMPLE_ANY_SAMPLE_RATE -DCHIP_HAS_TRANSQ -DRTOS=1 \
            -DCQ_FUNC_ATTR= -DDEBUG=1 -DPERSIST_DATA_SECTION_SIZE=0x1000

$(NAME)_CFLAGS += $(MACROS)

GLOBAL_DEFINES += STDIO_UART=0 CLI_CONFIG_STACK_SIZE=8192
#GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_MMC
#GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT
#GLOBAL_DEFINES += LITTLEFS_FORMAT
GLOBAL_DEFINES += RHINO_CONFIG_TICKS_PER_SECOND=1000
GLOBAL_DEFINES += POSIX_DEVICE_IO_NEED

export A7_SRAM ?= 2
ifeq ($(A7_SRAM),0)
export LASTWORD_RAM_ADDR ?= 0x201e6000
export DSP_RAM_SHARE_SIZE ?= 0x15000
GLOBAL_CFLAGS += -DDSP_RAM_SHARE_SIZE=$(DSP_RAM_SHARE_SIZE)
else ifeq ($(A7_SRAM),1)
export LASTWORD_RAM_ADDR ?= 0x20166000
GLOBAL_CFLAGS += -DDSP_SRAM_SOLO
else
export LASTWORD_RAM_ADDR ?= 0x20166000
endif
GLOBAL_CFLAGS += -DLASTWORD_RAM_ADDR=$(LASTWORD_RAM_ADDR)
GLOBAL_ASMFLAGS += -DLASTWORD_RAM_ADDR=$(LASTWORD_RAM_ADDR)

export A7_DSP_ENABLE ?= 0
ifeq ($(A7_DSP_ENABLE),1)
GLOBAL_CFLAGS += -D__A7_DSP_ENABLE__
endif

$(NAME)_INCLUDES += ../../mcu/drivers/apps/main

$(NAME)_ASMFLAGS += -c -x assembler-with-cpp

ifeq ($(MBINS),)
GLOBAL_LDFLAGS += -T vendor/haas/mcu/drivers/_haas1000_alios_sec_b.lds
else ifeq ($(MBINS),app)
GLOBAL_LDFLAGS += -T vendor/haas/board/haas100-mk/$(MBINS_APP).ld.S
else
GLOBAL_LDFLAGS += -T vendor/haas/mcu/drivers/_haas1000_alios_sec_b.lds
endif

GLOBAL_INCLUDES += ./config ./drivers

#CURRENT_TIME = $(shell /bin/date +%Y%m%d.%H%M)
#define get-os-version
#"AOS-R"-$(CURRENT_TIME)
#endef

ifeq ($(shell uname -o), Msys)
CURRENT_TIME = $(shell ${DATE} +%Y%m%d.%H%M)
endif

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_HAAS100
CONFIG_SYSINFO_DEVICE_NAME := HAAS100
CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"Cortex-M4F\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"
GLOBAL_CFLAGS += -DSYSINFO_BOARD=\"$(MODULE)\"
GLOBAL_CFLAGS += -DOS_CLOCK_NOMINAL=16000
GLOBAL_CFLAGS += -DAOS_COMP_NETMGR_USPACE_RUNABLE=1

#warning all
GLOBAL_CFLAGS += -Wall

#-Werror -Wall -w