/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_TYPES_H__
#define __UVOICE_TYPES_H__


typedef enum {
	MEDIA_FMT_UNKNOWN = 0,
	MEDIA_FMT_PCM,
	MEDIA_FMT_WAV,
	MEDIA_FMT_MP3,
	MEDIA_FMT_AAC,
	MEDIA_FMT_M4A,
	MEDIA_FMT_OGG,
	MEDIA_FMT_OPS,
	MEDIA_FMT_SPX,
	MEDIA_FMT_WMA,
	MEDIA_FMT_AMR,
	MEDIA_FMT_AMRWB,
	MEDIA_FMT_FLAC,
	MEDIA_FMT_COUNT,
} media_format_t;

typedef enum {
	AUDIO_OUT_DEVICE_SPEAKER = 1,
	AUDIO_OUT_DEVICE_HEADPHONE,
	AUDIO_OUT_DEVICE_HEADSET,
	AUDIO_OUT_DEVICE_RECEIVER,
	AUDIO_OUT_DEVICE_SPEAKER_AND_HEADPHONE,
	AUDIO_OUT_DEVICE_SPEAKER_AND_HEADSET,
	AUDIO_OUT_DEVICE_MAX,
} audio_out_device_t;

typedef struct {
	int used;
	int active_high;
	int pin;
	int delay_ms;
} audio_extpa_info_t;

typedef struct {
	char name[32];
	char author[32];
	char album[32];
	char year[8];
	int valid;
	unsigned char type;
	int32_t bitrate;
	int32_t media_size;
	int32_t duration;
} media_info_t;

typedef struct {
	int place; /* 0: none, 1: file, 2: mem */
	int mem_size; /* cache memory size in KB */
	char file_path[128]; /* cache file full path */
} cache_config_t;


#endif /* __UVOICE_TYPES_H__ */
