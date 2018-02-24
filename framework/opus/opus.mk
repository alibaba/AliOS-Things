NAME := opus


GLOBAL_INCLUDES  :=./include 
$(NAME)_INCLUDES  :=./ 
$(NAME)_DEFINES := HAVE_CONFIG_H 

#$(NAME)_CFLAGS +=-DOVERRIDE_OPUS_ALLOC -DOVERRIDE_OPUS_FREE -D'opus_alloc(x)=NULL' -D'opus_free(x)=NULL'
CELT_SOURCES = celt/bands.c celt/celt.c celt/celt_encoder.c \
	celt/celt_decoder.c celt/cwrs.c celt/entcode.c celt/entdec.c \
	celt/entenc.c celt/kiss_fft.c celt/laplace.c celt/mathops.c \
	celt/mdct.c celt/modes.c celt/pitch.c celt/celt_lpc.c \
	celt/quant_bands.c celt/rate.c celt/vq.c $(am__append_6) \
	$(am__append_7) $(am__append_8) $(am__append_9) \
	$(am__append_11) $(am__append_13)
CELT_SOURCES_SSE = celt/x86/x86cpu.c \
celt/x86/x86_celt_map.c \
celt/x86/pitch_sse.c

CELT_SOURCES_SSE2 = celt/x86/pitch_sse2.c
CELT_SOURCES_SSE4_1 = celt/x86/celt_lpc_sse.c \
celt/x86/pitch_sse4_1.c

CELT_SOURCES_ARM = \
celt/arm/armcpu.c \
celt/arm/arm_celt_map.c

CELT_SOURCES_ARM_ASM = \
celt/arm/celt_pitch_xcorr_arm.s

CELT_AM_SOURCES_ARM_ASM = \
celt/arm/armopts.s.in

CELT_SOURCES_ARM_NEON_INTR = \
celt/arm/celt_neon_intr.c

CELT_SOURCES_ARM_NE10 = \
celt/arm/celt_ne10_fft.c \
celt/arm/celt_ne10_mdct.c

SILK_SOURCES = silk/CNG.c silk/code_signs.c silk/init_decoder.c \
	silk/decode_core.c silk/decode_frame.c \
	silk/decode_parameters.c silk/decode_indices.c \
	silk/decode_pulses.c silk/decoder_set_fs.c silk/dec_API.c \
	silk/enc_API.c silk/encode_indices.c silk/encode_pulses.c \
	silk/gain_quant.c silk/interpolate.c silk/LP_variable_cutoff.c \
	silk/NLSF_decode.c silk/NSQ.c silk/NSQ_del_dec.c silk/PLC.c \
	silk/shell_coder.c silk/tables_gain.c silk/tables_LTP.c \
	silk/tables_NLSF_CB_NB_MB.c silk/tables_NLSF_CB_WB.c \
	silk/tables_other.c silk/tables_pitch_lag.c \
	silk/tables_pulses_per_block.c silk/VAD.c \
	silk/control_audio_bandwidth.c silk/quant_LTP_gains.c \
	silk/VQ_WMat_EC.c silk/HP_variable_cutoff.c silk/NLSF_encode.c \
	silk/NLSF_VQ.c silk/NLSF_unpack.c silk/NLSF_del_dec_quant.c \
	silk/process_NLSFs.c silk/stereo_LR_to_MS.c \
	silk/stereo_MS_to_LR.c silk/check_control_input.c \
	silk/control_SNR.c silk/init_encoder.c silk/control_codec.c \
	silk/A2NLSF.c silk/ana_filt_bank_1.c silk/biquad_alt.c \
	silk/bwexpander_32.c silk/bwexpander.c silk/debug.c \
	silk/decode_pitch.c silk/inner_prod_aligned.c silk/lin2log.c \
	silk/log2lin.c silk/LPC_analysis_filter.c \
	silk/LPC_inv_pred_gain.c silk/table_LSF_cos.c silk/NLSF2A.c \
	silk/NLSF_stabilize.c silk/NLSF_VQ_weights_laroia.c \
	silk/pitch_est_tables.c silk/resampler.c \
	silk/resampler_down2_3.c silk/resampler_down2.c \
	silk/resampler_private_AR2.c silk/resampler_private_down_FIR.c \
	silk/resampler_private_IIR_FIR.c \
	silk/resampler_private_up2_HQ.c silk/resampler_rom.c \
	silk/sigm_Q15.c silk/sort.c silk/sum_sqr_shift.c \
	silk/stereo_decode_pred.c silk/stereo_encode_pred.c \
	silk/stereo_find_predictor.c silk/stereo_quant_pred.c \
	$(am__append_1) $(am__append_2) $(am__append_3) \
	$(am__append_4) $(am__append_10) $(am__append_12)
SILK_SOURCES_SSE4_1 = silk/x86/NSQ_sse.c \
silk/x86/NSQ_del_dec_sse.c \
silk/x86/x86_silk_map.c \
silk/x86/VAD_sse.c \
silk/x86/VQ_WMat_EC_sse.c

SILK_SOURCES_ARM_NEON_INTR = \
silk/arm/arm_silk_map.c \
silk/arm/NSQ_neon.c

SILK_SOURCES_FIXED = \
silk/fixed/LTP_analysis_filter_FIX.c \
silk/fixed/LTP_scale_ctrl_FIX.c \
silk/fixed/corrMatrix_FIX.c \
silk/fixed/encode_frame_FIX.c \
silk/fixed/find_LPC_FIX.c \
silk/fixed/find_LTP_FIX.c \
silk/fixed/find_pitch_lags_FIX.c \
silk/fixed/find_pred_coefs_FIX.c \
silk/fixed/noise_shape_analysis_FIX.c \
silk/fixed/prefilter_FIX.c \
silk/fixed/process_gains_FIX.c \
silk/fixed/regularize_correlations_FIX.c \
silk/fixed/residual_energy16_FIX.c \
silk/fixed/residual_energy_FIX.c \
silk/fixed/solve_LS_FIX.c \
silk/fixed/warped_autocorrelation_FIX.c \
silk/fixed/apply_sine_window_FIX.c \
silk/fixed/autocorr_FIX.c \
silk/fixed/burg_modified_FIX.c \
silk/fixed/k2a_FIX.c \
silk/fixed/k2a_Q16_FIX.c \
silk/fixed/pitch_analysis_core_FIX.c \
silk/fixed/vector_ops_FIX.c \
silk/fixed/schur64_FIX.c \
silk/fixed/schur_FIX.c

SILK_SOURCES_FIXED_SSE4_1 = silk/fixed/x86/vector_ops_FIX_sse.c \
silk/fixed/x86/burg_modified_FIX_sse.c \
silk/fixed/x86/prefilter_FIX_sse.c

SILK_SOURCES_FLOAT = \
silk/float/apply_sine_window_FLP.c \
silk/float/corrMatrix_FLP.c \
silk/float/encode_frame_FLP.c \
silk/float/find_LPC_FLP.c \
silk/float/find_LTP_FLP.c \
silk/float/find_pitch_lags_FLP.c \
silk/float/find_pred_coefs_FLP.c \
silk/float/LPC_analysis_filter_FLP.c \
silk/float/LTP_analysis_filter_FLP.c \
silk/float/LTP_scale_ctrl_FLP.c \
silk/float/noise_shape_analysis_FLP.c \
silk/float/prefilter_FLP.c \
silk/float/process_gains_FLP.c \
silk/float/regularize_correlations_FLP.c \
silk/float/residual_energy_FLP.c \
silk/float/solve_LS_FLP.c \
silk/float/warped_autocorrelation_FLP.c \
silk/float/wrappers_FLP.c \
silk/float/autocorrelation_FLP.c \
silk/float/burg_modified_FLP.c \
silk/float/bwexpander_FLP.c \
silk/float/energy_FLP.c \
silk/float/inner_product_FLP.c \
silk/float/k2a_FLP.c \
silk/float/levinsondurbin_FLP.c \
silk/float/LPC_inv_pred_gain_FLP.c \
silk/float/pitch_analysis_core_FLP.c \
silk/float/scale_copy_vector_FLP.c \
silk/float/scale_vector_FLP.c \
silk/float/schur_FLP.c \
silk/float/sort_FLP.c

OPUS_SOURCES = src/opus.c src/opus_decoder.c src/opus_encoder.c \
	src/opus_multistream.c src/opus_multistream_encoder.c \
	src/opus_multistream_decoder.c src/repacketizer.c \
	$(am__append_5)
OPUS_SOURCES_FLOAT = \
src/analysis.c \
src/mlp.c \
src/mlp_data.c

##noinst_LTLIBRARIES = libarmasm.la
##libarmasm_la_SOURCES = $(CELT_SOURCES_ARM_ASM:.s=-gnu.S)


$(NAME)_INCLUDES  +=./src ./silk   ./celt 



opus_fixed:=1
opus_no_float:=1
ifeq ($(opus_fixed),1)
$(NAME)_DEFINES  += FIXED_POINT 
am__append_1 = $(SILK_SOURCES_FIXED)
$(NAME)_INCLUDES  += ./silk/fixed
endif


ifeq ($(opus_no_float),1)
$(NAME)_DEFINES  += DISABLE_FLOAT_API 
else
am__append_3 = $(SILK_SOURCES_FLOAT)
am__append_5 = $(OPUS_SOURCES_FLOAT)
$(NAME)_SOURCES += $(OPUS_SOURCES_FLOAT) 
$(NAME)_INCLUDES  += ./silk/float
endif

ifeq ($(HOST_ARCH), linux)
GLOBAL_DEFINES  += CPU_TYPE_X86  
GLOBAL_CFLAGS    += -Wall -msse4.1 -mavx
am__append_6 = $(CELT_SOURCES_SSE)
am__append_7 = $(CELT_SOURCES_SSE2)
am__append_8 = $(CELT_SOURCES_SSE4_1)
am__append_4 = $(SILK_SOURCES_SSE4_1)
$(NAME)_INCLUDES  += ./silk/x86
ifeq ($(opus_fixed),1)
am__append_2 =  $(SILK_SOURCES_FIXED_SSE4_1)
endif
else 

ifeq ($(HOST_ARCH), Cortex-M4)
$(NAME)_DEFINES  += CPU_TYPE_ARM  OPUS_ARM_ASM 
am__append_9 = $(CELT_SOURCES_ARM)
am__append_10 = $(SILK_SOURCES_ARM)
$(NAME)_INCLUDES  += ./silk/arm
##am__append_11 = $(CELT_SOURCES_ARM_NEON_INTR)
##am__append_12 = $(SILK_SOURCES_ARM_NEON_INTR)
##am__append_13 = $(CELT_SOURCES_ARM_NE10)
am__append_14 = libarmasm.la
else 
$(NAME)_DEFINES  += CPU_TYPE_ARM  OPUS_ARM_ASM 
am__append_9 = $(CELT_SOURCES_ARM)
am__append_10 = $(SILK_SOURCES_ARM)
$(NAME)_INCLUDES  += ./silk/arm
##am__append_11 = $(CELT_SOURCES_ARM_NEON_INTR)
##am__append_12 = $(SILK_SOURCES_ARM_NEON_INTR)
##am__append_13 = $(CELT_SOURCES_ARM_NE10)
am__append_14 = libarmasm.la
endif
endif

                   
$(NAME)_SOURCES := $(CELT_SOURCES) $(SILK_SOURCES) $(OPUS_SOURCES)


#$(NAME)_COMPONENTS :=opus113/silk  opus113/celt




