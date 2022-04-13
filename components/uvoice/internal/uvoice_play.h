/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_PLAY_H__
#define __UVOICE_PLAY_H__


typedef enum {
	PLAYER_MSG_PCM_INFO = 0,
	PLAYER_MSG_MEDIA_INFO,
	PLAYER_MSG_SEEK_DONE,
	PLAYER_MSG_LOAD_BLOCK,
} player_message_t;

typedef enum {
	STREAM_MGR_STAT_IDLE = 0,
	STREAM_MGR_STAT_READY,
	STREAM_MGR_STAT_RUNNING,
	STREAM_MGR_STAT_STOP,
} stream_mgr_state_t;

typedef enum {
	FADE_NOP = 0,
	FADE_IN_END,
	FADE_OUT_END,
	FADE_IN,
	FADE_OUT,
} fade_state_t;

typedef enum {
	SEEK_NOP = 0,
	SEEK_PREPARE,
	SEEK_START,
	SEEK_COMPLETE,
} seek_state_t;

typedef enum {
	PLAYER_CLOSE = 0,
	PLAYER_PAUSE,
	PLAYER_RESUME,
	PLAYER_START,
	PLAYER_NEXT,
	PLAYER_STOP,
	PLAYER_COMPLETE,
	PLAYER_SEEK_BEGIN,
	PLAYER_SEEK,
	PLAYER_SEEK_END,
	PLAYER_RELOAD,
	PLAYER_DLOAD,
	PLAYER_DLOAD_ABORT,
	PLAYER_UNBLOCK,
	PLAYER_CONFIGURE,
	STREAM_MGR_START,
	STREAM_MGR_STOP,
} player_action_t;

typedef struct {
	int dirty_cache_size;
	int avail_cache_size;
	int seek_forward_limit;
	int seek_backward_limit;
} cache_info_t;

typedef struct {
	media_format_t format;
	uint8_t *buffer_out;
	int buffer_out_size;
	int unproc_size;
	int input_size;
	uint8_t initialized:1;
	uint8_t running:1;
	uint8_t stere_enable:1;
	uint8_t pos_rebase:1;
	uint32_t rebase_offset;
	int (*decode)(void *priv, uint8_t *buffer, int nbytes);
	int (*action)(void *priv, player_action_t action, void *arg);
	int (*message)(void *priv, player_message_t msg, void *arg);
	int (*output)(void *priv, uint8_t *buffer, int nbytes);
	int (*reset)(void *priv);
	void *decoder;
	void *priv;
} media_decoder_t;

typedef struct {
	media_type_t type;
	uint8_t *buffer;
	int buffer_size;
	int buffer_dirty_size;
	int rebase_request;
	int rebase_offset;
	int (*read)(void *priv, uint8_t *buffer, int nbytes);
	int (*action)(void *priv, player_action_t action, void *arg);
	int (*reset)(void *priv);
	int (*message)(void *priv, player_message_t msg, void *arg);
	int (*get_format)(void *priv, media_format_t *format);
	int (*get_mediainfo)(void *priv, media_info_t *info, media_format_t format);
	int (*get_cacheinfo)(void *priv, cache_info_t *info);
	void *loader;
	void *priv;
} media_loader_t;

typedef struct {
	uvoice_ringbuff_t rb;
	uint8_t *buffer;
	uint8_t *stream_pool;
	int buffer_size;
	int buffer_dirty_size;
	int stream_pool_size;
	int wr_len;
	int rd_len;
	uint8_t cache_enable:1;
	uint8_t wr_waiting:1;
	uint8_t rd_waiting:1;
	uint8_t cplt_waiting:1;
	uint8_t stop:1;
	uint8_t abort:1;
	media_format_t format;
	os_sem_t cplt_sem;
	os_sem_t wr_sem;
	os_sem_t rd_sem;
	media_decoder_t *mdecoder;
	struct out_stream *out;
	stream_mgr_state_t state;
	os_mutex_t lock;
	os_task_t task;
	media_pcminfo_t pcm_info;
} stream_mgr_t;

typedef struct {
	int out_scope;
	int out_period_ms;
	int in_scope;
	int in_period_ms;
	int pos;
	int reset;
	fade_state_t state;
	os_mutex_t lock;
} fade_process_t;

typedef struct {
	int offset;
	int fade_request;
	seek_state_t state;
} seek_process_t;

typedef struct {
	int src_channels;
	int dst_channels;
	int src_bits;
	int dst_bits;
	int coeff;
	uint8_t *buffer;
	int buffer_size;
} format_convert_t;

typedef struct {
	uint32_t pause:1;
	uint32_t stop:1;
	uint32_t complete:1;
	uint32_t error:1;
	uint32_t pcm_reset:1;
	uint32_t stream_flowing:1;
	uint32_t cplt_waiting:1;
	uint32_t start_waiting:1;
	uint32_t pause_waiting:1;
	uint32_t resume_waiting:1;
	uint32_t stop_waiting:1;
	uint32_t sw_volume:1;
	uint32_t fade_enable:1;
	uint32_t fade_ignore:1;
	uint32_t blocking:1;
	uint32_t deep_cplt:1;
	uint32_t frame_skip:1;
	uint32_t out_bypass:1;
	uint32_t vol_level;
	fade_process_t fade;
	seek_process_t seek;
	media_format_t format;
	media_format_t format_preferable;
	media_type_t type;
	player_state_t state;
	long long silent_time;
	int consumed_len;
	int idle_period;
	int standby_msec;
	int reference_count;
	os_mutex_t lock;
	os_mutex_t vol_lock;
	os_sem_t start_sem;
	os_sem_t pause_sem;
	os_sem_t resume_sem;
	os_sem_t cplt_sem;
	os_sem_t reset_sem;
	os_sem_t stop_sem;
	format_convert_t *convertor;
	media_decoder_t *mdecoder;
	media_loader_t *mloader;
	stream_mgr_t *stream_mgr;
	struct out_stream *out;
	void *resampler;
	media_info_t *media_info;
	media_pcminfo_t pcm_info;
	os_task_t task;
} player_t;

int media_loader_reset(media_loader_t *mloader);
media_loader_t *media_loader_create(media_type_t type, char *source);
int media_loader_release(media_loader_t *mloader);
int media_decoder_reset(media_decoder_t *mdecoder);
media_decoder_t *media_decoder_create(media_format_t format);
int media_decoder_release(media_decoder_t *mdecoder);


#endif /* __UVOICE_PLAY_H__ */

