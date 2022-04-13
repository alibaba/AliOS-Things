ifneq ($(GEN_SDK_LIB),1)
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))

FILE_LIST := tgt_hardware.c

ifneq ($(SLAVE_BIN_FLASH_OFFSET),)
FILE_LIST += slave_code.S
$(obj)/$(DEFAULT_CFG_SRC)/slave_code.o : out/$(SLAVE_BIN_TARGET_NAME)/$(SLAVE_BIN_TARGET_NAME).bin
AFLAGS_slave_code.o += -DSLAVE_BIN_NAME=$(SLAVE_BIN_TARGET_NAME).bin -Iout/$(SLAVE_BIN_TARGET_NAME)
endif

obj-y := $(patsubst $(cur_dir)%,%,$(foreach f,$(FILE_LIST), \
    $(if $(wildcard $(cur_dir)$(T)/$(f)),$(cur_dir)$(T)/$(f),$(cur_dir)$(DEFAULT_CFG_SRC)/$(f))))

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
obj-y := $(obj-y:.S=.o)


else


cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libconfig
ifneq ($(SDK),1)

#obj-y := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c $(cur_dir)*.cpp $(cur_dir)*.S))

FILE_LIST := tgt_hardware.c

ifneq ($(SLAVE_BIN_FLASH_OFFSET),)
FILE_LIST += slave_code.S
$(obj)/$(DEFAULT_CFG_SRC)/slave_code.o : out/$(SLAVE_BIN_TARGET_NAME)/$(SLAVE_BIN_TARGET_NAME).bin
AFLAGS_slave_code.o += -DSLAVE_BIN_NAME=$(SLAVE_BIN_TARGET_NAME).bin -Iout/$(SLAVE_BIN_TARGET_NAME)
endif

obj-y := $(patsubst $(cur_dir)%,%,$(foreach f,$(FILE_LIST), \
    $(if $(wildcard $(cur_dir)$(T)/$(f)),$(cur_dir)$(T)/$(f),$(cur_dir)$(DEFAULT_CFG_SRC)/$(f))))

obj-y := $(obj-y:.c=.o)
obj-y := $(obj-y:.cpp=.o)
src_obj := $(obj-y:.S=.o)

$(LIB_NAME)-y := $(src_obj)
endif

obj-y := $(LIB_NAME).a
endif

ccflags-y += \
	-Iplatform/hal \
	-Iplatform/drivers/usb/usb_dev/inc \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Iservices/multimedia/speech/inc \
	-Iservices/multimedia/audio/process/filters/include \
	-Iservices/multimedia/audio/process/drc/include \
	-Iservices/multimedia/audio/process/limiter/include

ifeq ($(BT_DEV_NAME),)
BT_DEV_NAME := $(T).$(BUILD_HOSTNAME).$(BUILD_USERNAME)
endif

ccflags-y += -DBT_DEV_NAME=$(BT_DEV_NAME) $(CUST_TGT_FLAGS)

ifneq ($(BT_DEV_ADDR),)
ccflags-y += -DBT_DEV_ADDR=$(BT_DEV_ADDR)
endif

ifneq ($(BLE_DEV_ADDR),)
ccflags-y += -DBT_DEV_ADDR=$(BLE_DEV_ADDR)
endif

ifneq ($(USB_AUDIO_SEND_CHAN),)
ccflags-y += -DUSB_AUDIO_SEND_CHAN=$(USB_AUDIO_SEND_CHAN)
endif

ifeq ($(SPEECH_TX_AEC_CODEC_REF),1)
ccflags-y += -DSPEECH_TX_AEC_CODEC_REF
endif

ifeq ($(ANC_NOISE_TRACKER),1)
ccflags-y += -DANC_NOISE_TRACKER
endif
