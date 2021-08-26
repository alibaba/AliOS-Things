cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj-y := $(obj-y:.c=.o)

ifeq ($(MCU_HIGH_PERFORMANCE_MODE),1)
ifneq ($(FLASH_LOW_SPEED),1)
ifneq ($(OSC_26M_X4_AUD2BB),1)
#$(error FLASH_LOW_SPEED should be used with MCU_HIGH_PERFORMANCE_MODE when OSC_26M_X4_AUD2BB is NOT enabled on $(CHIP))
endif
endif
endif

ifeq ($(PMU_IRQ_UNIFIED),1)
CMU_CFG_FLAGS += -DPMU_IRQ_UNIFIED
endif

ifneq ($(MCU_HIGH_PERFORMANCE_MODE), 0)
CMU_CFG_FLAGS += -DMCU_HIGH_PERFORMANCE_MODE_$(MCU_HIGH_PERFORMANCE_MODE)
endif

ifeq ($(USB_USE_USBPLL),1)
CMU_CFG_FLAGS += -DUSB_USE_USBPLL
endif

ifeq ($(AUDIO_USE_BBPLL),1)
CMU_CFG_FLAGS += -DAUDIO_USE_BBPLL
endif

ifeq ($(SYS_USE_USBPLL),1)
CMU_CFG_FLAGS += -DSYS_USE_USBPLL
endif
ifeq ($(SYS_USE_BB_PSRAMPLL),1)
CMU_CFG_FLAGS += -DSYS_USE_BB_PSRAMPLL
endif
ifeq ($(SYS_USE_DSPPLL),1)
CMU_CFG_FLAGS += -DSYS_USE_DSPPLL
endif

ifeq ($(DSP_USE_BBPLL),1)
CMU_CFG_FLAGS += -DDSP_USE_BBPLL
endif
ifeq ($(DSP_USE_USBPLL),1)
CMU_CFG_FLAGS += -DDSP_USE_USBPLL
endif

CMU_CFG_FLAGS +=-DA7_DSP_SPEED=$(A7_DSP_SPEED)

ifeq ($(DAC_CLASSG_ENABLE),1)
CODEC_CFG_FLAGS += -DDAC_CLASSG_ENABLE
endif

ifeq ($(DAC_DRE_ENABLE),1)
CODEC_CFG_FLAGS += -DDAC_DRE_ENABLE
endif

ifeq ($(JTAG_BT),1)
PSC_CFG_FLAGS += -DJTAG_BT
JTAG_TDI_TDO_PIN ?= 1
endif

ifeq ($(JTAG_TDI_TDO_PIN),1)
IOMUX_CFG_FLAGS += -DJTAG_TDI_TDO_PIN
endif

ifeq ($(JTAG_RESET_PIN),1)
IOMUX_CFG_FLAGS += -DJTAG_RESET_PIN
endif

ifeq ($(USE_CYBERON),1)
ANAIF_CFG_FLAGS	+= -DUSE_CYBERON
endif

CMU_CFG_FLAGS += -Iplatform/drivers/ana/haas1000

CFLAGS_hal_analogif_$(CHIP).o += $(ANAIF_CFG_FLAGS)

CFLAGS_hal_cmu_$(CHIP).o += $(CMU_CFG_FLAGS)

CFLAGS_hal_psc_$(CHIP).o += $(PSC_CFG_FLAGS)

CFLAGS_hal_codec_$(CHIP).o += $(CODEC_CFG_FLAGS)

CFLAGS_hal_iomux_$(CHIP).o += $(IOMUX_CFG_FLAGS)

ccflags-y +=  -Iplatform/drivers/wifi/$(CHIP)
