NAME := cmsis_nn
$(NAME)_TYPE := third_party

GLOBAL_INCLUDES    +=   \
                        CMSIS_5/CMSIS/Core/Include \
                        CMSIS_5/CMSIS/DSP/Include \
                        CMSIS_5/CMSIS/NN/Include \

$(NAME)_SOURCES    += \
                        CMSIS_5/CMSIS/NN/Source/ActivationFunctions/arm_nn_activations_q7.c  \
                        CMSIS_5/CMSIS/NN/Source/ActivationFunctions/arm_relu_q15.c  \
                        CMSIS_5/CMSIS/NN/Source/ActivationFunctions/arm_relu6_s8.c  \
                        CMSIS_5/CMSIS/NN/Source/ActivationFunctions/arm_relu_q7.c  \
                        CMSIS_5/CMSIS/NN/Source/ActivationFunctions/arm_nn_activations_q15.c  \
                        CMSIS_5/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q7_opt.c  \
                        CMSIS_5/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q7.c  \
                        CMSIS_5/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q15.c  \
                        CMSIS_5/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_q15_opt.c  \
                        CMSIS_5/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15.c  \
                        CMSIS_5/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_s8.c  \
                        CMSIS_5/CMSIS/NN/Source/FullyConnectedFunctions/arm_fully_connected_mat_q7_vec_q15_opt.c  \
                        CMSIS_5/CMSIS/NN/Source/SoftmaxFunctions/arm_softmax_q15.c  \
                        CMSIS_5/CMSIS/NN/Source/SoftmaxFunctions/arm_softmax_q7.c  \
                        CMSIS_5/CMSIS/NN/Source/SoftmaxFunctions/arm_softmax_with_batch_q7.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_nn_accumulate_q7_to_q15.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_nn_add_q7.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_nntables.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_no_shift.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q15.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_reordered_with_offset.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_nn_mult_q7.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_no_shift.c  \
                        CMSIS_5/CMSIS/NN/Source/NNSupportFunctions/arm_q7_to_q15_with_offset.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_HWC_q7_basic.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_HWC_q7_basic_nonsquare.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_HWC_q7_fast.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_1x1_s8_fast.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_HWC_q7_fast_nonsquare.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_nn_mat_mult_kernel_s8_s16.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_HWC_q15_fast_nonsquare.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_1x1_HWC_q7_fast_nonsquare.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_HWC_q15_fast.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_HWC_q15_basic.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15_reordered.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_depthwise_conv_u8_basic_ver1.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_depthwise_conv_s8.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_depthwise_conv_s8_opt.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_s8.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_depthwise_separable_conv_HWC_q7_nonsquare.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_convolve_HWC_q7_RGB.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_nn_mat_mult_kernel_s8_s16_reordered.c  \
                        CMSIS_5/CMSIS/NN/Source/ConvolutionFunctions/arm_nn_mat_mult_kernel_q7_q15.c  \
                        CMSIS_5/CMSIS/NN/Source/PoolingFunctions/arm_max_pool_s8.c  \
                        CMSIS_5/CMSIS/NN/Source/PoolingFunctions/arm_avgpool_s8.c  \
                        CMSIS_5/CMSIS/NN/Source/PoolingFunctions/arm_pool_q7_HWC.c  \
                        CMSIS_5/CMSIS/NN/Source/PoolingFunctions/arm_max_pool_s8_opt.c  \
                        CMSIS_5/CMSIS/NN/Source/PoolingFunctions/arm_pool_q7_HWC_nonsquare.c  \
                        CMSIS_5/CMSIS/NN/Source/BasicMathFunctions/arm_elementwise_add_s8.c  \
                        CMSIS_5/CMSIS/NN/Source/BasicMathFunctions/arm_elementwise_mul_s8.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_dct4_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_rfft_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix4_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix2_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_bitreversal.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix4_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix8_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_bitreversal2.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_rfft_fast_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_dct4_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_dct4_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_rfft_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_dct4_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix4_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_rfft_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix2_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix4_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix2_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_dct4_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix4_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix4_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_dct4_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix2_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix2_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_rfft_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/TransformFunctions/arm_cfft_radix2_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/CommonTables/arm_const_structs.c  \
                        CMSIS_5/CMSIS/DSP/Source/CommonTables/arm_common_tables.c  \
                        CMSIS_5/CMSIS/DSP/Source/FastMathFunctions/arm_cos_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FastMathFunctions/arm_sin_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FastMathFunctions/arm_sqrt_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FastMathFunctions/arm_sqrt_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FastMathFunctions/arm_cos_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FastMathFunctions/arm_cos_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FastMathFunctions/arm_sin_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FastMathFunctions/arm_sin_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_conj_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_dot_prod_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mag_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mult_real_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mult_real_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mag_squared_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mag_squared_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mag_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_conj_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mag_squared_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_dot_prod_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mag_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_dot_prod_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_conj_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ComplexMathFunctions/arm_cmplx_mult_real_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_cmplx_mult_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_scale_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_sub_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_scale_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_scale_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_add_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_cmplx_mult_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_trans_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_mult_fast_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_mult_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_mult_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_inverse_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_add_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_mult_fast_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_inverse_f64.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_add_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_trans_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_trans_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_cmplx_mult_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_sub_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_mult_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/MatrixFunctions/arm_mat_sub_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_decimate_fast_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_iir_lattice_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_fast_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_sparse_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_opt_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_fast_opt_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_norm_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df2T_f64.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_norm_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_decimate_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_fast_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_norm_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_sparse_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_32x64_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_lattice_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_norm_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_norm_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_iir_lattice_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_decimate_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_interpolate_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df2T_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_fast_opt_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_sparse_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_interpolate_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_opt_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_fast_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_sparse_init_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_opt_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_sparse_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_lattice_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_fast_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_fast_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_lattice_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df2T_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_init_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_fast_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_interpolate_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_iir_lattice_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_sparse_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_fast_opt_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_iir_lattice_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_decimate_fast_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_sparse_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_interpolate_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_norm_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df2T_init_f64.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_decimate_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_lattice_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_fast_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_opt_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_fast_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_lattice_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_iir_lattice_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_decimate_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_partial_fast_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_32x64_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_interpolate_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_decimate_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_decimate_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_conv_opt_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_sparse_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_opt_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_interpolate_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_fir_lattice_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_iir_lattice_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_biquad_cascade_stereo_df2T_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_lms_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/FilteringFunctions/arm_correlate_fast_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_var_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_kullback_leibler_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_min_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_mean_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_power_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_min_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_std_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_mean_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_min_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_logsumexp_dot_prod_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_max_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_rms_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_entropy_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_power_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_max_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_rms_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_var_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_std_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_mean_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_power_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_std_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_var_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_min_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_max_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_mean_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_max_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_power_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_rms_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/StatisticsFunctions/arm_logsumexp_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BayesFunctions/arm_gaussian_naive_bayes_predict_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_russellrao_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_cosine_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_chebyshev_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_dice_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_minkowski_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_yule_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_correlation_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_euclidean_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_jaccard_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_rogerstanimoto_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_jensenshannon_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_braycurtis_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_cityblock_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_hamming_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_canberra_distance_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_kulsinski_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_boolean_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_sokalmichener_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/DistanceFunctions/arm_sokalsneath_distance.c  \
                        CMSIS_5/CMSIS/DSP/Source/SVMFunctions/arm_svm_polynomial_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SVMFunctions/arm_svm_rbf_predict_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SVMFunctions/arm_svm_sigmoid_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SVMFunctions/arm_svm_polynomial_predict_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SVMFunctions/arm_svm_sigmoid_predict_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SVMFunctions/arm_svm_rbf_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SVMFunctions/arm_svm_linear_predict_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SVMFunctions/arm_svm_linear_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_dot_prod_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_offset_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_add_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_add_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_offset_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_shift_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_mult_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_abs_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_mult_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_dot_prod_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_mult_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_sub_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_abs_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_scale_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_scale_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_sub_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_offset_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_dot_prod_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_shift_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_add_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_scale_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_offset_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_add_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_dot_prod_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_shift_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_sub_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_abs_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_mult_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_sub_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_negate_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_negate_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_negate_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_scale_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_negate_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/BasicMathFunctions/arm_abs_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_fill_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_copy_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q31_to_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_barycenter_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q15_to_float.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_fill_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_float_to_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q31_to_float.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_weighted_sum_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_float_to_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q7_to_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q15_to_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q15_to_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_copy_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_float_to_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_copy_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_fill_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q7_to_float.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q7_to_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_fill_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_copy_q7.c  \
                        CMSIS_5/CMSIS/DSP/Source/SupportFunctions/arm_q31_to_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ControllerFunctions/arm_sin_cos_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ControllerFunctions/arm_pid_init_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ControllerFunctions/arm_pid_init_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/ControllerFunctions/arm_pid_reset_q15.c  \
                        CMSIS_5/CMSIS/DSP/Source/ControllerFunctions/arm_pid_init_q31.c  \
                        CMSIS_5/CMSIS/DSP/Source/ControllerFunctions/arm_pid_reset_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/ControllerFunctions/arm_sin_cos_f32.c  \
                        CMSIS_5/CMSIS/DSP/Source/ControllerFunctions/arm_pid_reset_q31.c  \
