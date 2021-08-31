
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.s))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ccflags-y := \
	-Iservices/multimedia/audio/process/filters/include \
	-Iservices/multimedia/audio/process/common/include \
	-Iservices/multimedia/audio/process/drc/include \
	-Iservices/multimedia/audio/process/limiter/include \
	-Iservices/multimedia/speech/inc \
	-Iservices/nv_section/aud_section \
	-Iservices/nv_section/include \
	-Iutils/heap/

ifeq ($(ALSA_AUDIO_PROCESS_MODE_CHECK), 1)
ccflags-y += -Iservices/wifi_app/alsa_adapter/
endif

ifeq ($(EQ_DRC_IN_CP), 1)
ccflags-y += -Iservices/cp_server/
endif

ifeq ($(HW_FIR_EQ_PROCESS),1)
ccflags-y += -D__HW_FIR_EQ_PROCESS__
endif

ifeq ($(HW_IIR_EQ_PROCESS),1)
ccflags-y += -D__HW_IIR_EQ_PROCESS__
endif

ifeq ($(SW_IIR_EQ_PROCESS),1)
ccflags-y += -D__SW_IIR_EQ_PROCESS__
endif

ifeq ($(AUDIO_DRC),1)
ccflags-y += -D__AUDIO_DRC__
endif

ifeq ($(AUDIO_DRC2),1)
ccflags-y += -D__AUDIO_DRC2__
endif

ifeq ($(AUDIO_RESAMPLE),1)
ccflags-y += -D__AUDIO_RESAMPLE__
endif

ifeq ($(HW_DAC_IIR_EQ_PROCESS),1)
ccflags-y += -D__HW_DAC_IIR_EQ_PROCESS__
ifneq ($(filter best2300p best1400 best3001 best2300 best2000 best1000,$(CHIP)),)
ifneq ($(CHIP_SUBTYPE),best1402)
ifeq ($(ANC_APP),1)
$(error "error: ANC and Hardware codec iir can't be used in the same")
endif
endif
endif
endif

ifeq ($(FIR_HIGHSPEED),1)
ccflags-y += -DFIR_HIGHSPEED
endif

ifeq ($(PC_CMD_UART),1)
CFLAGS_audio_process.o += -D__PC_CMD_UART__
endif

ifeq ($(USB_AUDIO_APP), 1)
CFLAGS_audio_process.o += -DUSB_AUDIO_APP
endif

ifeq ($(SPEECH_LIB),1)
CFLAGS_audio_spectrum.o += -DSPEECH_LIB
endif

ifeq ($(USB_EQ_TUNING), 1)
ccflags-y += -DUSB_EQ_TUNING
endif

CFLAGS_audio_spectrum.o += -DUSE_MED_MALLOC
