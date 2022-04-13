cmake_minimum_required (VERSION 2.8)

project(uvoice)

option(target_os "set target os type")
option(target_host "set target host type")
option(uvoice_release "build uvoice release version" off)

link_libraries(-lpthread -lm)

link_directories(${PROJECT_SOURCE_DIR}/out/libs ${PROJECT_SOURCE_DIR}/out/prebuilt)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/libs)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/bin)

if (target_os STREQUAL "linux")
add_definitions(-D __os_linux__)
else()
message(FATAL_ERROR "target os type not specified !!!")
endif()

if (target_host STREQUAL "x64")
add_definitions(-D __cpu_x86_64__)
else()
message(FATAL_ERROR "target host type not specified !!!")
endif()

message("-- target os is ${target_os}")
message("-- target host is ${target_host}")

set(SRC_LIST
	test/test_main.c
	test/test_player.c
	test/test_recorder.c
	test/test_tts.c
	uvoice.c
	common/event.c
	common/ringbuffer.c
	common/message.c
	common/gbk2utf8.c
	media/uvoice_format.c
	media/uvoice_codec.c
	media/uvoice_stream.c
	media/uvoice_wave.c
	media/uvoice_fade.c
	media/uvoice_mlist.c
	media/uvoice_player.c
	media/uvoice_recorder.c
	stream/uvoice_file.c
	stream/uvoice_partition.c
	stream/uvoice_http.c
	stream/uvoice_cache.c
	stream/uvoice_download.c
	stream/uvoice_hls.c
	audio/audio_stream.c
	audio/audio_common.c
	audio/audio_mixer.c
	audio/audio_process.c
	audio/audio_trigger.c
	audio/process/proc_equalizer.c
	audio/process/proc_resample.c
	audio/hal/linux/uvoice_pcm.c
	application/alicloudtts/tts.c
	application/alicloudtts/alicloudtts.c)

set(SRC_CODEC_LIST
	codec/pvmp3_decoder.c
	codec/helixaac_decoder.c
	codec/ogg_decoder.c
	codec/flac_decoder.c
	codec/faad2aac_decoder.c
	codec/faad2m4a_decoder.c
	codec/opus_encoder.c
	codec/opus_decoder.c
	codec/spx_encoder.c
	codec/spx_decoder.c
	codec/amr_encoder.c
	codec/amr_decoder.c
	codec/amrwb_encoder.c
	codec/amrwb_decoder.c)

add_definitions(
	-D UVOICE_PROJECT
	-D UVOICE_PLAYER_ENABLE
	-D UVOICE_EVENT_ENABLE
	-D UVOICE_EVENT_BY_MESSAGE_QUEUE
	-D UVOICE_FILE_ENABLE
	-D UVOICE_PARTITION_ENABLE
	-D UVOICE_HTTP_ENABLE
	-D UVOICE_MLIST_ENABLE
	-D AUDIO_CACHE_ENABLE
	-D UVOICE_HLS_ENABLE
	-D ALICLOUD_TTS_SUPPORT
	-D UVOICE_TTS_ENABLE
	-D UVOICE_RESAMPLE_ENABLE
	-D UVOICE_EQ_ENABLE
	-D UVOICE_DOWNLOAD_ENABLE
	-D UVOICE_RELEASE_VERSION_MAIN=1
	-D UVOICE_RELEASE_VERSION_SUB=0)

if (uvoice_release)
add_definitions(-D UVOICE_BUILD_RELEASE)
endif()

add_definitions(
	-D DECODER_PV_MP3_ENABLE
	-D DECODER_OGG_ENABLE
	-D DECODER_FLAC_ENABLE
	-D DECODER_APE_ENABLE
	-D ENCODER_OPUS_ENABLE
	-D DECODER_OPUS_ENABLE
	-D ENCODER_SPEEX_ENABLE
	-D DECODER_SPEEX_ENABLE
	-D ENCODER_AMR_ENABLE
	-D DECODER_AMR_ENABLE
	-D ENCODER_AMRWB_ENABLE
	-D DECODER_AMRWB_ENABLE
	-D DECODER_FAAD2_M4A_ENABLE
	-D DECODER_FAAD2_AAC_ENABLE)

include_directories(
	include
	internal
	audio
	codec
	connect
	application/alicloudtts/include)

add_subdirectory(codec/opensource/pvmp3)
#add_subdirectory(codec/opensource/helixaac)
#add_subdirectory(codec/opensource/libogg)
#add_subdirectory(codec/opensource/libvorbis)
#add_subdirectory(codec/opensource/flac)
#add_subdirectory(codec/opensource/faad2)
#add_subdirectory(codec/opensource/opus)
#add_subdirectory(codec/opensource/speex)
#add_subdirectory(codec/opensource/amr)
#add_subdirectory(codec/opensource/amrwb)
#add_subdirectory(codec/opensource/wma)

#add_subdirectory(audio/process/opensource/libsox)
add_subdirectory(audio/process/opensource/speexdsp)

add_executable(uvoice ${SRC_LIST} ${SRC_CODEC_LIST})

if (target_os STREQUAL "linux")
target_link_libraries(uvoice asound)
endif()

target_link_libraries(uvoice pvmp3)
target_link_libraries(uvoice helixaac)
target_link_libraries(uvoice vorbis)
target_link_libraries(uvoice ogg)
target_link_libraries(uvoice flac)
target_link_libraries(uvoice faad2)
target_link_libraries(uvoice opus)
target_link_libraries(uvoice speex)
target_link_libraries(uvoice amr)
target_link_libraries(uvoice amrwb)
target_link_libraries(uvoice wma)
target_link_libraries(uvoice resampler)
target_link_libraries(uvoice sox)
