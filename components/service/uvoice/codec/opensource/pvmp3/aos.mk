NAME := pvmp3

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := pvmp3

GLOBAL_DEFINES     += DECODER_PVMP3_ENABLE

ifeq ($(strip $(HOST_ARCH)),$(filter $(HOST_ARCH),xtensa ARM968E-S))
#GLOBAL_DEFINES     += PV_ARM_GCC_V5
endif

GLOBAL_INCLUDES    += include/

$(NAME)_SOURCES    := \
                   src/pvmp3_normalize.c \
                   src/pvmp3_alias_reduction.c \
                   src/pvmp3_crc.c \
                   src/pvmp3_decode_header.c \
                   src/pvmp3_decode_huff_cw.c \
                   src/pvmp3_getbits.c \
                   src/pvmp3_dequantize_sample.c \
                   src/pvmp3_framedecoder.c \
                   src/pvmp3_get_main_data_size.c \
                   src/pvmp3_get_side_info.c \
                   src/pvmp3_get_scale_factors.c \
                   src/pvmp3_mpeg2_get_scale_data.c \
                   src/pvmp3_mpeg2_get_scale_factors.c \
                   src/pvmp3_mpeg2_stereo_proc.c \
                   src/pvmp3_huffman_decoding.c \
                   src/pvmp3_huffman_parsing.c \
                   src/pvmp3_tables.c \
                   src/pvmp3_imdct_synth.c \
                   src/pvmp3_mdct_6.c \
                   src/pvmp3_dct_6.c \
                   src/pvmp3_poly_phase_synthesis.c \
                   src/pvmp3_equalizer.c \
                   src/pvmp3_seek_synch.c \
                   src/pvmp3_stereo_proc.c \
                   src/pvmp3_reorder.c \
                   src/pvmp3_polyphase_filter_window.c \
                   src/pvmp3_mdct_18.c \
                   src/pvmp3_dct_9.c \
                   src/pvmp3_dct_16.c

