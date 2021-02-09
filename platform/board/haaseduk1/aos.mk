NAME := board_haaseduk1

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board haasedu k1
MODULE             := HAASEDUK1
HOST_ARCH          := Cortex-M4
HOST_MCU_FAMILY    := mcu_haas1000
HOST_MCU_NAME      := haas1000
SUPPORT_MBINS      := no
#ENABLE_VFP         := 0
export SECURE_OTA_BOOT    := yes

#use script to make littlefs bin
#export MAKE_DATA_IMAGE	:= yes

ifneq ($(AOS_2NDBOOT_SUPPORT),yes)

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init yloop debug  usb rtc_ext

GLOBAL_DEFINES += STDIO_UART=0 CLI_CONFIG_STACK_SIZE=8192
#GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_MMC
#GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT
#GLOBAL_DEFINES += LITTLEFS_FORMAT
GLOBAL_DEFINES += RHINO_CONFIG_TICKS_PER_SECOND=1000

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

GLOBAL_DEFINES += POSIX_DEVICE_IO_NEED
GLOBAL_DEFINES += I2C_ITF_SET

$(NAME)_SOURCES += config/k_config.c \
                   config/partition_conf.c \
                   config/board.c \
                   startup/startup.c \
                   drivers/led.c \
				   drivers/key.c \
                   drivers/watchdog.c \
                   drivers/exp_adc.c 
                   
MACROS := -DCHIP_HAAS1000 -DCHIP_HAS_UART=3 -DRESAMPLE_ANY_SAMPLE_RATE -DCHIP_HAS_TRANSQ -DRTOS=1 \
			-DCQ_FUNC_ATTR= -DDEBUG=1 -DPERSIST_DATA_SECTION_SIZE=0x1000

$(NAME)_CFLAGS += $(MACROS)

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

$(NAME)_INCLUDES += ../../mcu/haas1000/drivers/apps/main
$(NAME)_INCLUDES += ../../../../kernel/drivers/lattice

$(NAME)_ASMFLAGS += -c -x assembler-with-cpp

GLOBAL_LDFLAGS += -T platform/mcu/haas1000/drivers/_haas1000_alios_sec_b.lds

GLOBAL_INCLUDES += ./config ./drivers



CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_HAASEDUK1
CONFIG_SYSINFO_DEVICE_NAME := HAASEDUK1
CONFIG_SYSINFO_OS_VERSION := $(call get-os-version)

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"
GLOBAL_CFLAGS += -DSYSINFO_ARCH=\"Cortex-M4F\"
GLOBAL_CFLAGS += -DSYSINFO_MCU=\"$(HOST_MCU_FAMILY)\"
GLOBAL_CFLAGS += -DSYSINFO_BOARD=\"$(MODULE)\"
GLOBAL_CFLAGS += -DOS_CLOCK_NOMINAL=16000

# Extra build target include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
ifeq ($(SECURE_OTA_BOOT), yes)
EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/release/auto_build_tool/haas1000_genbin.mk
endif

else
GLOBAL_DEFINES += AOS_2ND_BOOT_NO_LDS AOS_2ND_BOOT_AB AOS_2ND_BOOT_LATER_ERASE
$(NAME)_SOURCES += config/partition_conf.c

$(NAME)_COMPONENTS += ota_2ndboot
EXTRA_TARGET_MAKEFILES +=  $($(HOST_MCU_FAMILY)_LOCATION)/release/auto_build_tool/haas1000_2ndboot_genbin.mk
endif
