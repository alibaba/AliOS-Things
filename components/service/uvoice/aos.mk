NAME := uvoice

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := Audio framework

uvoice_build_type ?= release

$(NAME)_DEFINES    += UVOICE_RELEASE_VERSION_MAIN=1
$(NAME)_DEFINES    += UVOICE_RELEASE_VERSION_SUB=0

GLOBAL_DEFINES     += AOS_UVOICE UVOICE_PROJECT __os_alios_things__

GLOBAL_LDFLAGS     += -lm

ifeq ($(uvoice_build_type),release)
GLOBAL_DEFINES     += UVOICE_BUILD_RELEASE
else
GLOBAL_DEFINES     += UVOICE_BUILD_DEBUG
endif

$(NAME)_COMPONENTS := kv a2sa

GLOBAL_INCLUDES    += include test

GLOBAL_INCLUDES    += connect

$(NAME)_INCLUDES   += . internal audio

$(NAME)_SOURCES    := \
                   uvoice.c \
                   common/event.c \
                   common/ringbuffer.c \
				   common/urlencode.c \
                   media/uvoice_format.c \
                   media/uvoice_codec.c \
                   media/uvoice_stream.c \
                   media/uvoice_wave.c \
                   audio/audio_stream.c \
                   audio/audio_common.c \
                   audio/audio_mixer.c \
                   audio/audio_process.c \
                   audio/audio_trigger.c

ifeq ($(uvoice_player_enable),1)
GLOBAL_DEFINES     += UVOICE_PLAYER_ENABLE
$(NAME)_SOURCES    += \
                   media/uvoice_fade.c \
                   media/uvoice_player.c \
                   test/test_player.c
endif

ifeq ($(uvoice_recorder_enable),1)
GLOBAL_DEFINES     += UVOICE_RECORDER_ENABLE
$(NAME)_SOURCES    += \
                   media/uvoice_recorder.c \
                   test/test_recorder.c
endif

ifeq ($(uvoice_mlist_enable),1)
GLOBAL_DEFINES     += UVOICE_MLIST_ENABLE
$(NAME)_SOURCES    += media/uvoice_mlist.c
endif

ifeq ($(uvoice_event_enable),1)
GLOBAL_DEFINES     += UVOICE_EVENT_ENABLE
endif
GLOBAL_DEFINES     += UVOICE_EVENT_BY_BUFFER_QUEUE

ifeq ($(uvoice_file_enable),1)
GLOBAL_DEFINES     += UVOICE_FILE_ENABLE
$(NAME)_SOURCES    += stream/uvoice_file.c
endif

ifeq ($(uvoice_http_enable),1)
GLOBAL_DEFINES     += UVOICE_HTTP_ENABLE
$(NAME)_SOURCES    += \
                   stream/uvoice_http.c \
                   stream/uvoice_cache.c
endif

ifeq ($(uvoice_https_enable),1)
GLOBAL_DEFINES     += UVOICE_HTTPS_ENABLE
$(NAME)_SOURCES    += stream/uvoice_ssl.c
endif

ifeq ($(uvoice_download_enable),1)
GLOBAL_DEFINES     += UVOICE_DOWNLOAD_ENABLE
$(NAME)_SOURCES    += stream/uvoice_download.c
endif

ifeq ($(uvoice_ws_enable),1)
GLOBAL_DEFINES     += UVOICE_WS_ENABLE
$(NAME)_SOURCES    += connect/uvoice_ws.c
endif

ifeq ($(uvoice_hls_enable),1)
GLOBAL_DEFINES     += UVOICE_HLS_ENABLE
$(NAME)_SOURCES    += stream/uvoice_hls.c
endif

ifeq ($(uvoice_partition_enable),1)
GLOBAL_DEFINES     += UVOICE_PARTITION_ENABLE
$(NAME)_SOURCES    += stream/uvoice_partition.c
endif


ifeq ($(uvoice_anc_enable),1)
GLOBAL_DEFINES     += UVOICE_ANC_ENABLE
$(NAME)_SOURCES    += audio/process/proc_ns.c
$(NAME)_COMPONENTS += components/service/uvoice/audio/process/opensource/webrtc
endif

ifeq ($(uvoice_aec_enable),1)
GLOBAL_DEFINES     += UVOICE_AEC_ENABLE
$(NAME)_SOURCES    += \
                   audio/audio_aec.c \
                   audio/process/proc_aec.c
$(NAME)_COMPONENTS += components/service/uvoice/audio/process/opensource/webrtc
endif

ifeq ($(uvoice_agc_enable),1)
GLOBAL_DEFINES     += UVOICE_AGC_ENABLE
$(NAME)_SOURCES    += audio/process/proc_agc.c
$(NAME)_COMPONENTS += components/service/uvoice/audio/process/opensource/webrtc
endif

ifeq ($(uvoice_vad_enable),1)
GLOBAL_DEFINES     += UVOICE_VAD_ENABLE
$(NAME)_SOURCES    += \
                   audio/audio_vad.c \
                   audio/process/proc_vad.c
$(NAME)_COMPONENTS += components/service/uvoice/audio/process/opensource/webrtc
endif

ifeq ($(uvoice_eq_enable),1)
GLOBAL_DEFINES     += UVOICE_EQ_ENABLE
$(NAME)_SOURCES    += \
                   audio/process/proc_equalizer.c
$(NAME)_COMPONENTS += components/service/uvoice/audio/process/opensource/libsox
endif

ifeq ($(uvoice_resample_enable),1)
GLOBAL_DEFINES     += UVOICE_RESAMPLE_ENABLE
$(NAME)_SOURCES    += audio/process/proc_resample.c
$(NAME)_COMPONENTS += components/service/uvoice/audio/process/opensource/speexdsp
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), XR871)
$(NAME)_SOURCES    += audio/hal/xr871/uvoice_pcm.c
GLOBAL_DEFINES     += UVOICE_ON_XR871
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), BK7221U)
$(NAME)_SOURCES    += audio/hal/bk7251/uvoice_pcm.c
GLOBAL_DEFINES     += UVOICE_ON_BK7251 AUDIO_CACHE_ENABLE
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), developerkit)
$(NAME)_SOURCES    += audio/hal/developerkit/uvoice_pcm.c
GLOBAL_DEFINES     += AUDIO_CACHE_ENABLE
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), XM530)
$(NAME)_SOURCES    += audio/hal/xm530/uvoice_pcm.c
GLOBAL_DEFINES     += AUDIO_CACHE_ENABLE
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), ASR3601)
$(NAME)_SOURCES    += audio/hal/asr3601/uvoice_pcm.c
GLOBAL_DEFINES     += UVOICE_ON_ASR3601 AUDIO_CACHE_ENABLE
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), LINUXHOST)
$(NAME)_SOURCES    += audio/hal/linuxhost/uvoice_pcm.c
GLOBAL_DEFINES     += AUDIO_CACHE_ENABLE
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), HAAS100) 
$(NAME)_SOURCES    += audio/hal/haas100/uvoice_pcm.c
GLOBAL_DEFINES     += AUDIO_CACHE_ENABLE
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), HAAS101) 
$(NAME)_SOURCES    += audio/hal/haas100/uvoice_pcm.c
GLOBAL_DEFINES     += AUDIO_CACHE_ENABLE
else ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), HAASEDUK1) 
$(NAME)_SOURCES    += audio/hal/haas100/uvoice_pcm.c
GLOBAL_DEFINES     += AUDIO_CACHE_ENABLE
endif


ifeq ($(dec_mad_enable),1)
$(NAME)_SOURCES    += codec/mad_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/mad
endif

ifeq ($(dec_pvmp3_enable),1)
$(NAME)_SOURCES    += codec/pvmp3_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/pvmp3
endif

ifeq ($(dec_pvaac_enable),1)
$(NAME)_SOURCES    += codec/pvaac_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/pvaac
endif

ifeq ($(dec_faad2m4a_enable),1)
$(NAME)_SOURCES    += codec/faad2m4a_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/faad2
endif

ifeq ($(dec_faad2aac_enable),1)
$(NAME)_SOURCES    += codec/faad2aac_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/faad2
endif

ifeq ($(dec_helixmp3_enable),1)
$(NAME)_SOURCES    += codec/helixmp3_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/helixmp3
endif

ifeq ($(dec_helixaac_enable),1)
$(NAME)_SOURCES    += codec/helixaac_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/helixaac
endif

ifeq ($(dec_helixm4a_enable),1)
$(NAME)_SOURCES    += codec/helixm4a_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/helixaac
endif

ifeq ($(enc_lame_enable),1)
$(NAME)_SOURCES    += codec/lame_encoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/lame
endif

ifeq ($(enc_speex_enable),1)
$(NAME)_SOURCES    += codec/spx_encoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/speex
endif

ifeq ($(dec_speex_enable),1)
$(NAME)_SOURCES    += codec/spx_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/speex
endif

ifeq ($(enc_opus_enable),1)
$(NAME)_SOURCES    += codec/opus_encoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/opus
endif

ifeq ($(dec_opus_enable),1)
$(NAME)_SOURCES    += codec/opus_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/opus
endif

ifeq ($(enc_amr_enable),1)
$(NAME)_SOURCES    += codec/amr_encoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/amr
endif

ifeq ($(dec_amr_enable),1)
$(NAME)_SOURCES    += codec/amr_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/amr
endif

ifeq ($(enc_amrwb_enable),1)
$(NAME)_SOURCES    += codec/amrwb_encoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/amrwb
endif

ifeq ($(dec_amrwb_enable),1)
$(NAME)_SOURCES    += codec/amrwb_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/amrwb
endif

ifeq ($(dec_faac_enable),1)
$(NAME)_SOURCES    += codec/faac_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/faac
endif

ifeq ($(dec_ogg_enable),1)
$(NAME)_SOURCES    += codec/ogg_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/libogg
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/libvorbis
endif

ifeq ($(dec_flac_enable),1)
$(NAME)_SOURCES    += codec/flac_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/flac
endif

ifeq ($(dec_wma_enable),1)
$(NAME)_SOURCES    += codec/wma_decoder.c
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/wma
endif


ifeq ($(uvoice_nopoll_enable),1)
$(NAME)_COMPONENTS += components/service/uvoice/3rdparty/nopoll
endif

ifeq ($(uvoice_mbedtls_enable),1)
GLOBAL_DEFINES     += UVOICE_MBEDTLS_ENABLE
$(NAME)_COMPONENTS += components/service/uvoice/3rdparty/uvoice_mbedtls
else
ifeq ($(uvoice_https_enable),1)
$(NAME)_COMPONENTS += mbedtls
endif
endif

ifeq ($(uvoice_comb_enable),1)
GLOBAL_DEFINES     += UVOICE_COMB_ENABLE
GLOBAL_INCLUDES    += application/comb/include
$(NAME)_SOURCES    += application/comb/comb.c
endif

ifeq ($(uvoice_swid_enable),1)
GLOBAL_DEFINES     += UVOICE_SWID_ENABLE
GLOBAL_INCLUDES    += application/swid/include
$(NAME)_SOURCES    += \
                   application/swid/swid.c \
                   test/test_swid.c
endif

ifeq ($(uvoice_offlineasr_enable),1)
GLOBAL_INCLUDES    += application/offlineasr/
$(NAME)_SOURCES    += application/offlineasr/asr_offline.c
endif

ifeq ($(uvoice_tts_enable),1)
GLOBAL_DEFINES     += ALICLOUD_TTS_SUPPORT
GLOBAL_DEFINES     += UVOICE_TTS_ENABLE
GLOBAL_INCLUDES    += application/alicloudtts/include
$(NAME)_SOURCES    += \
                   application/alicloudtts/tts.c \
                   application/alicloudtts/alicloudtts.c
$(NAME)_SOURCES    += test/test_tts.c
endif

ifeq ($(uvoice_kwd_enable),1)
$(NAME)_COMPONENTS += components/service/uvoice/application/kwd
endif

ifeq ($(uvoice_wetalk_enable),1)
GLOBAL_DEFINES     += UVOICE_WETALK_ENABLE
GLOBAL_INCLUDES    += application/wetalk/include
$(NAME)_SOURCES    += application/wetalk/wetalk.c
$(NAME)_SOURCES    += test/test_wetalk.c
endif

ifeq ($(uvoice_migu_enable),1)
$(NAME)_COMPONENTS += components/service/uvoice/application/musicplayer/musicsdk/migu/prebuilt
GLOBAL_DEFINES     += UVOICE_MIGU_MUSIC_ENABLE
GLOBAL_DEFINES     += UVOICE_DEEP_COMPLETE
$(NAME)_SOURCES    += application/musicplayer/musicsdk/migu/migu_api.c

else ifeq ($(uvoice_netease_enable),1)
GLOBAL_DEFINES     += UVOICE_NETEASE_MUSIC_ENABLE
$(NAME)_SOURCES    += application/musicplayer/musicsdk/netease/netease_api.c
endif

ifeq ($(uvoice_musicplayer_enable),1)
GLOBAL_DEFINES     += UVOICE_MUSICPLAYER_ENABLE
GLOBAL_INCLUDES    += application/musicplayer/include
GLOBAL_INCLUDES    += application/musicplayer/musicsdk/include
$(NAME)_SOURCES    += application/musicplayer/musicplayer.c
endif

ifeq ($(uvoice_iotnlp_enable),1)
GLOBAL_DEFINES     += UVOICE_IOTNLP_ENABLE
GLOBAL_INCLUDES    += application/iotnlp/include
$(NAME)_SOURCES    += \
                   common/base64.c \
                   application/iotnlp/iotnlp.c
endif

# Build Aligenie SDK

ifeq ($(aligenie_enable),1)
GLOBAL_DEFINES     += ALIGENIE_ENABLE
GLOBAL_INCLUDES    += application/aligenie/include
$(NAME)_COMPONENTS += components/service/uvoice/application/aligenie/prebuilt
$(NAME)_COMPONENTS += components/service/uvoice/codec/opensource/speex
$(NAME)_SOURCES    += \
                   application/aligenie/aligenie_init.c \
                   application/aligenie/adapt/agic.c \
                   application/aligenie/adapt/aligenie_os.c \
                   application/aligenie/adapt/aligenie_notify.c \
                   application/aligenie/adapt/aligenie_audio.c \
                   application/aligenie/adapt/aligenie_ws.c
endif

ifeq ($(mit_nui_enable),1)
GLOBAL_DEFINES     += MIT_NUI_ENABLE
GLOBAL_INCLUDES    += application/mitnui/include
$(NAME)_COMPONENTS += components/service/uvoice/application/mitnui/prebuilt
$(NAME)_SOURCES    += test/test_mitnui.c
endif

