
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

MULTIMEDIA_LIB_NAME := $(CHIP)_libmultimedia

ifeq ($(ROM_UTILS_ON),1)
MULTIMEDIA_LIB_NAME := $(MULTIMEDIA_LIB_NAME)_romaac
endif

ifeq ($(A2DP_CP_ACCEL),1)
MULTIMEDIA_LIB_NAME := $(MULTIMEDIA_LIB_NAME)_cp
endif

ifeq ($(ANC_APP),1)
MULTIMEDIA_LIB_NAME := $(MULTIMEDIA_LIB_NAME)_anc
endif

ifneq ($(SDK),1)
mmlib_obj :=

mmlib_obj += algorithm/fft/src/

mmlib_obj += audio/process/adp/src/
mmlib_obj += audio/process/anc/src/
mmlib_obj += audio/process/fir2iir/src/
ifeq ($(EQ_DRC_IN_CP), 0)
mmlib_obj += audio/process/common/src/
endif
mmlib_obj += audio/process/drc/src/
mmlib_obj += audio/process/filters/src/
mmlib_obj += audio/process/limiter/src/
mmlib_obj += audio/process/resample/src/
mmlib_obj += audio/process/floatlimiter/src/

ifeq ($(FM_ENABLE),1)
mmlib_obj += fm/
endif

ifeq ($(A2DP_OPUS_ON),1)
#mmlib_obj += opus121/src/
endif

ifeq ($(BT_APP),1)
mmlib_obj += audio/codec/sbc/src/
endif

ifeq ($(A2DP_AAC_ON),1)
aac_obj :=
aac_obj += audio/codec/fdkaac_codec/libAACdec/src/
aac_obj += audio/codec/fdkaac_codec/libAACenc/src/
aac_obj += audio/codec/fdkaac_codec/libFDK/src/
aac_obj += audio/codec/fdkaac_codec/libMpegTPDec/src/
aac_obj += audio/codec/fdkaac_codec/libMpegTPEnc/src/
aac_obj += audio/codec/fdkaac_codec/libPCMutils/src/
aac_obj += audio/codec/fdkaac_codec/libSBRdec/src/
aac_obj += audio/codec/fdkaac_codec/libSBRenc/src/
aac_obj += audio/codec/fdkaac_codec/libSYS/src/
ifeq ($(A2DP_CP_ACCEL),1)
fdkaac_codec-y := $(aac_obj)
mmlib_obj += fdkaac_codec.o
else
mmlib_obj += $(aac_obj)
endif
ifeq ($(ROM_UTILS_ON),1)
subdir-ccflags-y += -DFDK_FUNC_ATTR=WEAK
else
subdir-ccflags-y += -DFDK_FUNC_ATTR=
endif
endif

# for wifi app
ifeq ($(OPUS_CODEC),1)
mmlib_obj += audio/codec/opus/src/
endif

# for wifi app
ifeq ($(PORTAUDIO),1)
# mmlib_obj += portaudio/
endif

mmlib_obj += audio/codec/minimp3/

#mmlib_obj += rbcodec/src/

ifeq ($(SPEECH_LIB),1)
mmlib_obj += speech/src/
endif

$(MULTIMEDIA_LIB_NAME)-y := $(mmlib_obj)
endif

rel_src_obj :=
rel_src_obj += audio/process/anc/cfg/
rel_src_obj += audio/process/filters/cfg/
rel_src_obj += audio/process/resample/coef/

obj-y := $(MULTIMEDIA_LIB_NAME).a $(rel_src_obj)

subdir-ccflags-y += \
	-Iutils/heap \
	-Iinclude/cmsis_dsp \
	-Iinclude/cmsis_nn \
	-Iservices/cp_server

