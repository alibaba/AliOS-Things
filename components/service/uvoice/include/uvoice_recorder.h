/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __UVOICE_RECORDER_H__
#define __UVOICE_RECORDER_H__


typedef enum {
	RECORDER_STAT_IDLE = 0,
	RECORDER_STAT_READY,
	RECORDER_STAT_RUNNING,
	RECORDER_STAT_STOP,
	RECORDER_STAT_ERROR,
} recorder_state_t;

typedef struct {
	int (*set_sink)(media_format_t format, int rate, int channels, int bits, int frames, int bitrate, char *sink);
	int (*clr_sink)(void);
	int (*start)(void);
	int (*stop)(void);
	int (*get_stream)(uint8_t *buffer, int nbytes);
	int (*get_state)(recorder_state_t *state);
	int (*get_position)(int *position);
	int (*ns_enable)(int enable);
	int (*ec_enable)(int enable);
	int (*agc_enable)(int enable);
	int (*vad_enable)(int enable);
	int (*format_support)(media_format_t format);
	void *priv;
} uvoice_recorder_t;

uvoice_recorder_t *uvoice_recorder_create(void);
int uvoice_recorder_release(uvoice_recorder_t *mrecorder);


#endif /* __UVOICE_RECORDER_H__ */

