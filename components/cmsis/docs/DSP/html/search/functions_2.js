var searchData=
[
  ['clip_5fq31_5fto_5fq15',['clip_q31_to_q15',['../arm__math_8h.html#a8648908e55b9fd8893b36eb7050e69a8',1,'arm_math.h']]],
  ['clip_5fq31_5fto_5fq7',['clip_q31_to_q7',['../arm__math_8h.html#afa69cfc57f04c36c2ba499525168fe0c',1,'arm_math.h']]],
  ['clip_5fq63_5fto_5fq15',['clip_q63_to_q15',['../arm__math_8h.html#a027d446e8a72228f0a681779a7038a4e',1,'arm_math.h']]],
  ['clip_5fq63_5fto_5fq31',['clip_q63_to_q31',['../arm__math_8h.html#ad3a8d07983196592f6edca8f84aec7c7',1,'arm_math.h']]],
  ['cmake_5fminimum_5frequired',['cmake_minimum_required',['../Examples_2ARM_2arm__variance__example_2CMakeLists_8txt.html#aacae80504fd1beabc4cd793b87cbe4e2',1,'cmake_minimum_required(VERSION 3.6) project(arm_variance_example VERSION 0.1) list(APPEND CMAKE_MODULE_PATH $:&#160;CMakeLists.txt'],['../Source_2CMakeLists_8txt.html#a881c069b0c3dd34941126127656e23a1',1,'cmake_minimum_required(VERSION 3.6) project(CMSISDSP) list(APPEND CMAKE_MODULE_PATH $:&#160;CMakeLists.txt'],['../Source_2CommonTables_2CMakeLists_8txt.html#ab2ff652a443e047ef6441f2eeaae8048',1,'cmake_minimum_required(VERSION 3.6) project(CMSISDSPCommon) add_library(CMSISDSPCommon STATIC arm_common_tables.c) if(CONFIGTABLE AND ALLFFT) target_compile_definitions(CMSISDSPCommon PUBLIC ARM_ALL_FFT_TABLES) endif() if(CONFIGTABLE AND ALLFAST) target_compile_definitions(CMSISDSPCommon PUBLIC ARM_ALL_FAST_TABLES) endif() include(fft) fft(CMSISDSPCommon) include(interpol) interpol(CMSISDSPCommon) target_sources(CMSISDSPCommon PRIVATE arm_const_structs.c) configdsp(CMSISDSPCommon..) target_include_directories(CMSISDSPCommon PUBLIC&quot;$:&#160;CMakeLists.txt'],['../Source_2ControllerFunctions_2CMakeLists_8txt.html#a450cbc64f30b7f1182d8b66245ccd208',1,'cmake_minimum_required(VERSION 3.6) project(CMSISDSPController) add_library(CMSISDSPController STATIC) configdsp(CMSISDSPController..) include(interpol) interpol(CMSISDSPController) if(CONFIGTABLE AND ALLFAST) target_compile_definitions(CMSISDSPController PUBLIC ARM_ALL_FAST_TABLES) endif() target_sources(CMSISDSPController PRIVATE arm_pid_init_f32.c) target_sources(CMSISDSPController PRIVATE arm_pid_init_q15.c) target_sources(CMSISDSPController PRIVATE arm_pid_init_q31.c) target_sources(CMSISDSPController PRIVATE arm_pid_reset_f32.c) target_sources(CMSISDSPController PRIVATE arm_pid_reset_q15.c) target_sources(CMSISDSPController PRIVATE arm_pid_reset_q31.c) if(NOT CONFIGTABLE OR ALLFAST OR ARM_SIN_COS_F32) target_sources(CMSISDSPController PRIVATE arm_sin_cos_f32.c) endif() if(NOT CONFIGTABLE OR ALLFAST OR ARM_SIN_COS_Q31) target_sources(CMSISDSPController PRIVATE arm_sin_cos_q31.c) endif() target_include_directories(CMSISDSPController PUBLIC&quot;$:&#160;CMakeLists.txt'],['../Source_2FilteringFunctions_2CMakeLists_8txt.html#a459bd5973ca8abbfe2ffa2bd5ea5f0a8',1,'cmake_minimum_required(VERSION 3.6) project(CMSISDSPFiltering) add_library(CMSISDSPFiltering STATIC) include(interpol) interpol(CMSISDSPFiltering) configdsp(CMSISDSPFiltering..) if(CONFIGTABLE AND ALLFAST) target_compile_definitions(CMSISDSPFiltering PUBLIC ARM_ALL_FAST_TABLES) endif() if(NOT CONFIGTABLE OR ALLFAST OR ARM_LMS_NORM_Q31) target_sources(CMSISDSPFiltering PRIVATE arm_lms_norm_init_q31.c) endif() if(NOT CONFIGTABLE OR ALLFAST OR ARM_LMS_NORM_Q15) target_sources(CMSISDSPFiltering PRIVATE arm_lms_norm_init_q15.c) endif() target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_32x64_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_32x64_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_fast_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_fast_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_init_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df1_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df2T_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df2T_f64.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df2T_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_df2T_init_f64.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_stereo_df2T_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_biquad_cascade_stereo_df2T_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_fast_opt_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_fast_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_fast_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_opt_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_opt_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_fast_opt_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_fast_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_fast_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_opt_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_opt_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_partial_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_conv_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_fast_opt_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_fast_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_fast_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_opt_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_opt_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_correlate_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_decimate_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_decimate_fast_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_decimate_fast_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_decimate_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_decimate_init_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_decimate_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_decimate_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_decimate_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_fast_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_fast_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_init_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_init_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_interpolate_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_interpolate_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_interpolate_init_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_interpolate_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_interpolate_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_interpolate_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_lattice_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_lattice_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_lattice_init_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_lattice_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_lattice_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_lattice_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_sparse_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_sparse_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_sparse_init_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_sparse_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_sparse_init_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_sparse_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_sparse_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_fir_sparse_q7.c) target_sources(CMSISDSPFiltering PRIVATE arm_iir_lattice_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_iir_lattice_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_iir_lattice_init_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_iir_lattice_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_iir_lattice_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_iir_lattice_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_init_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_init_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_norm_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_norm_init_f32.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_norm_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_norm_q31.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_q15.c) target_sources(CMSISDSPFiltering PRIVATE arm_lms_q31.c) target_include_directories(CMSISDSPFiltering PUBLIC&quot;$:&#160;CMakeLists.txt'],['../Source_2TransformFunctions_2CMakeLists_8txt.html#ac12f00f4dcad3283ec1ce05fb627d05e',1,'cmake_minimum_required(VERSION 3.6) project(CMSISDSPTransform) add_library(CMSISDSPTransform STATIC) include(fft) fft(CMSISDSPTransform) if(CONFIGTABLE AND ALLFFT) target_compile_definitions(CMSISDSPTransform PUBLIC ARM_ALL_FFT_TABLES) endif() target_sources(CMSISDSPTransform PRIVATE arm_bitreversal.c) target_sources(CMSISDSPTransform PRIVATE arm_bitreversal2.c) if(NOT CONFIGTABLE OR ALLFFT OR CFFT_F32_16 OR CFFT_F32_32 OR CFFT_F32_64 OR CFFT_F32_128 OR CFFT_F32_256 OR CFFT_F32_512 OR CFFT_F32_1024 OR CFFT_F32_2048 OR CFFT_F32_4096) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix2_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix8_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_f32.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR CFFT_Q15_16 OR CFFT_Q15_32 OR CFFT_Q15_64 OR CFFT_Q15_128 OR CFFT_Q15_256 OR CFFT_Q15_512 OR CFFT_Q15_1024 OR CFFT_Q15_2048 OR CFFT_Q15_4096) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix2_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_q15.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR CFFT_Q31_16 OR CFFT_Q31_32 OR CFFT_Q31_64 OR CFFT_Q31_128 OR CFFT_Q31_256 OR CFFT_Q31_512 OR CFFT_Q31_1024 OR CFFT_Q31_2048 OR CFFT_Q31_4096) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix2_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_q31.c) endif() if(NOT CONFIGTABLE OR ALLFFT) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix2_init_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix2_init_q31.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR DCT4_F32_128 OR DCT4_F32_512 OR DCT4_F32_2048 OR DCT4_F32_8192) target_sources(CMSISDSPTransform PRIVATE arm_dct4_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_dct4_init_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_init_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_init_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_f32.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR DCT4_Q31_128 OR DCT4_Q31_512 OR DCT4_Q31_2048 OR DCT4_Q31_8192) target_sources(CMSISDSPTransform PRIVATE arm_dct4_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_dct4_init_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_init_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_init_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_q31.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR ALLFFT OR DCT4_Q15_128 OR DCT4_Q15_512 OR DCT4_Q15_2048 OR DCT4_Q15_8192) target_sources(CMSISDSPTransform PRIVATE arm_dct4_init_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_dct4_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_init_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_init_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_q15.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR RFFT_FAST_F32_32 OR RFFT_FAST_F32_64 OR RFFT_FAST_F32_128 OR RFFT_FAST_F32_256 OR RFFT_FAST_F32_512 OR RFFT_FAST_F32_1024 OR RFFT_FAST_F32_2048 OR RFFT_FAST_F32_4096) target_sources(CMSISDSPTransform PRIVATE arm_rfft_fast_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_fast_init_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix8_f32.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR RFFT_F32_128 OR RFFT_F32_512 OR RFFT_F32_2048 OR RFFT_F32_8192) target_sources(CMSISDSPTransform PRIVATE arm_rfft_init_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_init_f32.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_f32.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR RFFT_Q15_32 OR RFFT_Q15_64 OR RFFT_Q15_128 OR RFFT_Q15_256 OR RFFT_Q15_512 OR RFFT_Q15_1024 OR RFFT_Q15_2048 OR RFFT_Q15_4096 OR RFFT_Q15_8192) target_sources(CMSISDSPTransform PRIVATE arm_rfft_init_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_q15.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_q15.c) endif() if(NOT CONFIGTABLE OR ALLFFT OR RFFT_Q31_32 OR RFFT_Q31_64 OR RFFT_Q31_128 OR RFFT_Q31_256 OR RFFT_Q31_512 OR RFFT_Q31_1024 OR RFFT_Q31_2048 OR RFFT_Q31_4096 OR RFFT_Q31_8192) target_sources(CMSISDSPTransform PRIVATE arm_rfft_init_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_rfft_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_q31.c) target_sources(CMSISDSPTransform PRIVATE arm_cfft_radix4_q31.c) endif() configdsp(CMSISDSPTransform..) target_include_directories(CMSISDSPTransform PUBLIC&quot;$:&#160;CMakeLists.txt']]]
];
