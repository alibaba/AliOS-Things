/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_RECORD_H__
#define __UVOICE_RECORD_H__


typedef enum {
	RECORDER_CLOSE = 0,
	RECORDER_PAUSE,
	RECORDER_RESUME,
	RECORDER_START,
	RECORDER_STOP,
	RECORDER_UPDATE_HEAD,
} recorder_action_t;

typedef struct {
	media_type_t type;
	uint32_t size;
	int (*pack)(void *priv, uint8_t *buffer, int nbytes);
	int (*update)(void *priv, uint8_t *buffer, int nbytes, int pos);
	int (*action)(void *priv, recorder_action_t action, void *arg);
	void *packer;
} media_packer_t;

typedef struct {
	media_format_t format;
	void *encoder;
	void *header;
	void *tail;
	int header_size;
	int tail_size;
	uint8_t initialized:1;
	uint8_t header_pack:1;
	uint8_t header_cplt:1;
	uint8_t running:1;
	uint8_t vbr:1;
	int rate;
	int channels;
	int bits;
	int frames;
	int bitrate;
	int (*encode)(void *priv, uint8_t *buffer, int nbytes);
	int (*header_gen)(void *priv, void *arg);
	int (*header_update)(void *priv, int size);
	int (*action)(void *priv, recorder_action_t action, void *arg);
} media_encoder_t;

typedef struct {
	uint8_t *buffer;
	uint8_t *rec_buffer;
	int buffer_size;
	int buffer_dirty_size;
	int record_len;
	int reference_count;
	media_format_t format;
	media_type_t type;
	recorder_state_t state;
	uint8_t stop:1;
	uint8_t error:1;
	uint8_t ns_enable:1;
	uint8_t ec_enable:1;
	uint8_t agc_enable:1;
	uint8_t vad_enable:1;
	os_mutex_t lock;
	os_sem_t cplt_sem;
	int src_rate;
	int dst_rate;
	void *resampler;
	media_encoder_t *mencoder;
	media_packer_t *mpacker;
	struct in_stream *in;
	os_task_t task;
	media_pcminfo_t pcm_info;
} recorder_t;

media_packer_t *media_packer_create(char *sink, media_type_t type);
int media_packer_release(media_packer_t *mpacker);
int media_encoder_header_gen(media_encoder_t *mencoder, media_pcminfo_t *pcminfo);
media_encoder_t *media_encoder_create(media_format_t format);
int media_encoder_release(media_encoder_t *mencoder);


#endif /* __UVOICE_RECORD_H__ */

