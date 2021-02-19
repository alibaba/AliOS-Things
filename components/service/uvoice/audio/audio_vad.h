/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#ifndef __AUDIO_VAD_H__
#define __AUDIO_VAD_H__


enum {
	VAD_STAT_NOISE = 0,
	VAD_STAT_SPEECH,
};

struct voice_active_detect {
	void *handler;
	int state;
	int end_count;
	int end_target;
	long long start_time;
};

#ifdef UVOICE_VAD_ENABLE
int audio_vad_process(struct voice_active_detect *vad, const uint8_t *buffer, int nbytes);
struct voice_active_detect *audio_vad_create(int rate, int period_size);
int audio_vad_release(struct voice_active_detect *vad);
#endif


#endif /* __AUDIO_VAD_H__ */
