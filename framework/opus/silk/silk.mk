NAME := silk

$(NAME)_INCLUDES  := ./  ../include  arm  fixed float mips x86

SILK_SOURCES = \
CNG.c \
code_signs.c \
init_decoder.c \
decode_core.c \
decode_frame.c \
decode_parameters.c \
decode_indices.c \
decode_pulses.c \
decoder_set_fs.c \
dec_API.c \
enc_API.c \
encode_indices.c \
encode_pulses.c \
gain_quant.c \
interpolate.c \
LP_variable_cutoff.c \
NLSF_decode.c \
NSQ.c \
NSQ_del_dec.c \
PLC.c \
shell_coder.c \
tables_gain.c \
tables_LTP.c \
tables_NLSF_CB_NB_MB.c \
tables_NLSF_CB_WB.c \
tables_other.c \
tables_pitch_lag.c \
tables_pulses_per_block.c \
VAD.c \
control_audio_bandwidth.c \
quant_LTP_gains.c \
VQ_WMat_EC.c \
HP_variable_cutoff.c \
NLSF_encode.c \
NLSF_VQ.c \
NLSF_unpack.c \
NLSF_del_dec_quant.c \
process_NLSFs.c \
stereo_LR_to_MS.c \
stereo_MS_to_LR.c \
check_control_input.c \
control_SNR.c \
init_encoder.c \
control_codec.c \
A2NLSF.c \
ana_filt_bank_1.c \
biquad_alt.c \
bwexpander_32.c \
bwexpander.c \
debug.c \
decode_pitch.c \
inner_prod_aligned.c \
lin2log.c \
log2lin.c \
LPC_analysis_filter.c \
LPC_inv_pred_gain.c \
table_LSF_cos.c \
NLSF2A.c \
NLSF_stabilize.c \
NLSF_VQ_weights_laroia.c \
pitch_est_tables.c \
resampler.c \
resampler_down2_3.c \
resampler_down2.c \
resampler_private_AR2.c \
resampler_private_down_FIR.c \
resampler_private_IIR_FIR.c \
resampler_private_up2_HQ.c \
resampler_rom.c \
sigm_Q15.c \
sort.c \
sum_sqr_shift.c \
stereo_decode_pred.c \
stereo_encode_pred.c \
stereo_find_predictor.c \
stereo_quant_pred.c \
LPC_fit.c

SILK_SOURCES_SSE4_1 = x86/NSQ_sse.c \
x86/NSQ_del_dec_sse.c \
x86/x86_silk_map.c \
x86/VAD_sse.c \
x86/VQ_WMat_EC_sse.c

SILK_SOURCES_ARM_NEON_INTR = \
arm/arm_silk_map.c \
arm/biquad_alt_neon_intr.c \
arm/LPC_inv_pred_gain_neon_intr.c \
arm/NSQ_del_dec_neon_intr.c \
arm/NSQ_neon.c

SILK_SOURCES_FIXED = \
fixed/LTP_analysis_filter_FIX.c \
fixed/LTP_scale_ctrl_FIX.c \
fixed/corrMatrix_FIX.c \
fixed/encode_frame_FIX.c \
fixed/find_LPC_FIX.c \
fixed/find_LTP_FIX.c \
fixed/find_pitch_lags_FIX.c \
fixed/find_pred_coefs_FIX.c \
fixed/noise_shape_analysis_FIX.c \
fixed/process_gains_FIX.c \
fixed/regularize_correlations_FIX.c \
fixed/residual_energy16_FIX.c \
fixed/residual_energy_FIX.c \
fixed/warped_autocorrelation_FIX.c \
fixed/apply_sine_window_FIX.c \
fixed/autocorr_FIX.c \
fixed/burg_modified_FIX.c \
fixed/k2a_FIX.c \
fixed/k2a_Q16_FIX.c \
fixed/pitch_analysis_core_FIX.c \
fixed/vector_ops_FIX.c \
fixed/schur64_FIX.c \
fixed/schur_FIX.c

SILK_SOURCES_FIXED_SSE4_1 = fixed/x86/vector_ops_FIX_sse.c \
fixed/x86/burg_modified_FIX_sse.c

SILK_SOURCES_FIXED_ARM_NEON_INTR = \
fixed/arm/warped_autocorrelation_FIX_neon_intr.c

SILK_SOURCES_FLOAT = \
float/apply_sine_window_FLP.c \
float/corrMatrix_FLP.c \
float/encode_frame_FLP.c \
float/find_LPC_FLP.c \
float/find_LTP_FLP.c \
float/find_pitch_lags_FLP.c \
float/find_pred_coefs_FLP.c \
float/LPC_analysis_filter_FLP.c \
float/LTP_analysis_filter_FLP.c \
float/LTP_scale_ctrl_FLP.c \
float/noise_shape_analysis_FLP.c \
float/process_gains_FLP.c \
float/regularize_correlations_FLP.c \
float/residual_energy_FLP.c \
float/warped_autocorrelation_FLP.c \
float/wrappers_FLP.c \
float/autocorrelation_FLP.c \
float/burg_modified_FLP.c \
float/bwexpander_FLP.c \
float/energy_FLP.c \
float/inner_product_FLP.c \
float/k2a_FLP.c \
float/LPC_inv_pred_gain_FLP.c \
float/pitch_analysis_core_FLP.c \
float/scale_copy_vector_FLP.c \
float/scale_vector_FLP.c \
float/schur_FLP.c \
float/sort_FLP.c


$(NAME)_DEFINES := HAVE_CONFIG_H
$(NAME)_SOURCES := $(SILK_SOURCES) 

opus_fixed:=1
opus_no_float:=1


ifeq ($(opus_fixed),1)
$(NAME)_DEFINES  += FIXED_POINT 
$(NAME)_SOURCES += $(SILK_SOURCES_FIXED)
ifeq ($(HOST_ARCH), linux)
$(NAME)_SOURCES += $(SILK_SOURCES_FIXED_SSE4_1)
else
#$(NAME)_SOURCES += $(SILK_SOURCES_FIXED_ARM_NEON_INTR)
endif
endif

ifeq ($(opus_no_float),1)
$(NAME)_DEFINES  += DISABLE_FLOAT_API 
else
$(NAME)_SOURCES += $(SILK_SOURCES_FLOAT)
endif

ifeq ($(HOST_ARCH), linux)
HAVE_SSE4_1 :=1
ifdef HAVE_SSE4_1
$(NAME)_SOURCES +=  $(SILK_SOURCES_SSE4_1)
endif
else
ifeq ($(HOST_ARCH), Cortex-M4)

$(NAME)_DEFINES += OPUS_ARM_ASM CPU_TYPE_ARM
else 
ifeq ($(HOST_ARCH), Cortex-M4F)
$(NAME)_DEFINES  += CPU_TYPE_ARM  OPUS_ARM_ASM 
endif
endif
endif
