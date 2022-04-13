set(SRC_LIST
	src/pvmp3_normalize.c
	src/pvmp3_alias_reduction.c
	src/pvmp3_crc.c
	src/pvmp3_decode_header.c
	src/pvmp3_decode_huff_cw.c
	src/pvmp3_getbits.c
	src/pvmp3_dequantize_sample.c
	src/pvmp3_framedecoder.c
	src/pvmp3_get_main_data_size.c
	src/pvmp3_get_side_info.c
	src/pvmp3_get_scale_factors.c
	src/pvmp3_mpeg2_get_scale_data.c
	src/pvmp3_mpeg2_get_scale_factors.c
	src/pvmp3_mpeg2_stereo_proc.c
	src/pvmp3_huffman_decoding.c
	src/pvmp3_huffman_parsing.c
	src/pvmp3_tables.c
	src/pvmp3_imdct_synth.c
	src/pvmp3_mdct_6.c
	src/pvmp3_dct_6.c
	src/pvmp3_poly_phase_synthesis.c
	src/pvmp3_equalizer.c
	src/pvmp3_seek_synch.c
	src/pvmp3_stereo_proc.c
	src/pvmp3_reorder.c
	src/pvmp3_polyphase_filter_window.c
	src/pvmp3_mdct_18.c
	src/pvmp3_dct_9.c
	src/pvmp3_dct_16.c)

include_directories(
	include)

add_definitions(
	-D PV_ARM_MSC_EVC_V5)

add_library(pvmp3 STATIC ${SRC_LIST})

set_target_properties(pvmp3 PROPERTIES OUTPUT_NAME "pvmp3")

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/libs)

