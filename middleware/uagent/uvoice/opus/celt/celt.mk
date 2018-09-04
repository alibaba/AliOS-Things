
NAME := celt

$(NAME)_INCLUDES  := ./  ../include  arm  fixed float mips x86


$(NAME)_DEFINES := HAVE_CONFIG_H 

CELT_SOURCES = bands.c \
celt.c \
celt_encoder.c \
celt_decoder.c \
cwrs.c \
entcode.c \
entdec.c \
entenc.c \
kiss_fft.c \
laplace.c \
mathops.c \
mdct.c \
modes.c \
pitch.c \
celt_lpc.c \
quant_bands.c \
rate.c \
vq.c

CELT_SOURCES_SSE = \
x86/x86cpu.c \
x86/x86_celt_map.c \
x86/pitch_sse.c

CELT_SOURCES_SSE2 = \
x86/pitch_sse2.c \
x86/vq_sse2.c

CELT_SOURCES_SSE4_1 = \
x86/celt_lpc_sse.c \
x86/pitch_sse4_1.c

CELT_SOURCES_ARM = \
arm/armcpu.c \
arm/arm_celt_map.c

CELT_SOURCES_ARM_ASM = \
arm/celt_pitch_xcorr_arm.s

CELT_AM_SOURCES_ARM_ASM = \
arm/armopts.s.in

CELT_SOURCES_ARM_NEON_INTR = \
arm/celt_neon_intr.c \
arm/pitch_neon_intr.c

CELT_SOURCES_ARM_NE10 = \
arm/celt_ne10_fft.c \
arm/celt_ne10_mdct.c


$(NAME)_SOURCES := $(CELT_SOURCES) 

ifeq ($(HOST_ARCH), linux)
HAVE_SSE := 1
HAVE_SSE2 :=1
HAVE_SSE4_1:=1

ifneq ($(HAVE_SSE),)
$(NAME)_SOURCES +=  $(CELT_SOURCES_SSE) 
endif
ifneq ($(HAVE_SSE2),)
$(NAME)_SOURCES +=  $(CELT_SOURCES_SSE2)
endif
ifneq ($(HAVE_SSE4_1),)
$(NAME)_SOURCES +=  $(CELT_SOURCES_SSE4_1)
endif
else
ifeq ($(HOST_ARCH), Cortex-M4)
$(NAME)_SOURCES += $(CELT_SOURCES_ARM) 
$(NAME)_DEFINES := OPUS_ARM_ASM CPU_TYPE_ARM
else 
ifeq ($(HOST_ARCH), Cortex-M4F)
$(NAME)_SOURCES += $(CELT_SOURCES_ARM) 
$(NAME)_DEFINES  += CPU_TYPE_ARM  OPUS_ARM_ASM 
endif
endif
endif

opus_fixed:=1
opus_no_float:=1
ifeq ($(opus_no_float),1)
$(NAME)_DEFINES  += DISABLE_FLOAT_API 
endif

ifeq ($(opus_fixed),1)
$(NAME)_DEFINES  += FIXED_POINT 
endif
