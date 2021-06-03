/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
 
#ifndef __UVOICE_PLAYER_H__
#define __UVOICE_PLAYER_H__


typedef enum {
	PLAYER_STAT_IDLE = 0,
	PLAYER_STAT_READY,
	PLAYER_STAT_RUNNING,
	PLAYER_STAT_PAUSED,
	PLAYER_STAT_RESUME,
	PLAYER_STAT_STOP,
	PLAYER_STAT_COMPLETE,
	PLAYER_STAT_SEEK_CPLT,
	PLAYER_STAT_MEDIA_INFO,
	PLAYER_STAT_SOURCE_INVALID,
	PLAYER_STAT_FORMAT_UNSUPPORT,
	PLAYER_STAT_LIST_PLAY_START,
	PLAYER_STAT_LIST_PLAY_STOP,
	PLAYER_STAT_ERROR,
} player_state_t;

typedef struct {
	int (*start)(void);
	int (*stop)(void);
	int (*pause)(void);
	int (*resume)(void);
	int (*complete)(void);
	int (*stop_async)(void);
	int (*pause_async)(void);
	int (*resume_async)(void);
	int (*set_source)(char *source);
	int (*clr_source)(void);
	int (*set_stream)(media_format_t format, int cache_enable, int cache_size);
	int (*put_stream)(const uint8_t *buffer, int nbytes);
	int (*clr_stream)(int immediate);
	int (*play_list)(char **list);
	int (*set_pcminfo)(int rate, int channels, int bits, int frames);
	int (*get_duration)(int *duration);
	int (*get_position)(int *position);
	int (*set_volume)(int volume);
	int (*get_volume)(int *volume);
	int (*volume_range)(int *max, int *min);
	int (*seek)(int second);
	int (*playback)(char *source);
	int (*wait_complete)(void);
	int (*download)(char *name);
	int (*download_abort)(void);
	int (*cache_config)(cache_config_t *config);
	int (*set_fade)(int out_period, int in_period);
	int (*set_format)(media_format_t format);
	int (*set_out_device)(audio_out_device_t device);
	int (*set_external_pa)(audio_extpa_info_t *info);
	int (*set_standby)(int msec);
	int (*eq_enable)(int enable);
	int (*state_dump)(void);
	int (*pcmdump_enable)(int enable);
	int (*get_state)(player_state_t *state);
	int (*get_delay)(int *delay_ms);
	int (*get_mediainfo)(media_info_t *info);
	int (*get_cacheinfo)(int *cache_size);
	int (*format_support)(media_format_t format);
	void *priv;
} uvoice_player_t;

uvoice_player_t *uvoice_player_create(void);
int uvoice_player_release(uvoice_player_t *mplayer);


#endif /* __UVOICE_PLAYER_H__ */

