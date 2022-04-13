/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "uvoice_os.h"
#include "uvoice_types.h"
#include "uvoice_recorder.h"

#include "uvoice_common.h"
#include "uvoice_record.h"

#include "opensource/opus/source/include/opus.h"


#define UVOICE_OPUS_HEAD_SIZE		8


typedef struct {
	void *handler;
	short *in;
	char *out;
	int rate;
	int channels;
} opus_encoder_t;

static void int_to_char(uint32_t i, uint8_t ch[4])
{
    ch[0] = i>>24;
    ch[1] = (i>>16)&0xFF;
    ch[2] = (i>>8)&0xFF;
    ch[3] = i&0xFF;
}

static int opus_encode_process(void *priv, uint8_t *buffer, int nbytes)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	opus_encoder_t *opus;
	uint32_t enc_final_range = 0;
	char ch[4];
	int sample;
	int enc_len = 0;
	int out_len = 0;
	int len;
	int i;

	int time_ms = os_current_time();

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	opus = mencoder->encoder;
	if (!opus) {
		M_LOGE("opus encoder null !\n");
		return -1;
	}

	if (!opus->in) {
		opus->in = snd_zalloc(nbytes, AFM_MAIN);
		if (!opus->in) {
			M_LOGE("alloc opus in failed !\n");
			return -1;
		}
	}

	if (!opus->out) {
		opus->out = snd_zalloc(nbytes, AFM_MAIN);
		if (!opus->out) {
			M_LOGE("alloc opus out failed !\n");
			snd_free(opus->in);
			opus->in = NULL;
			return -1;
		}
	}

	for (i = 0; i < nbytes/sizeof(short); i++) {
		sample = buffer[2 * i + 1] << 8 | buffer[2 * i];
		opus->in[i] = ((sample & 0xffff) ^ 0x8000) - 0x8000;
	}

	while(1) {
		len = opus_encode(opus->handler, opus->in + enc_len,
			(nbytes - enc_len) / sizeof(short), opus->out, nbytes);
		if (len <= 0) {
			M_LOGE("encoder error %d !\n", len);
			return -1;
		}

		enc_len += opus_packet_get_samples_per_frame(opus->out, opus->rate) *
			opus_packet_get_nb_frames(opus->out, len);
		if (enc_len == 0) {
			M_LOGE("encoder output zero !\n");
			break;
		}

		snd_memcpy(buffer + out_len + UVOICE_OPUS_HEAD_SIZE,
			opus->out, len); /* playload */

		int_to_char(len, ch);
		snd_memcpy(buffer + out_len, ch, sizeof(int)); /* playload len */

		opus_encoder_ctl(opus->handler, OPUS_GET_FINAL_RANGE(&enc_final_range));
		int_to_char(enc_final_range, ch);
		snd_memcpy(buffer + out_len + UVOICE_OPUS_HEAD_SIZE - sizeof(int),
			ch, sizeof(int));

		out_len += len + UVOICE_OPUS_HEAD_SIZE;

		if (enc_len >= nbytes / sizeof(short))
			break;
	}

	//time_ms = os_current_time() - time_ms;
	//M_LOGD("time %dms ratio %d/%d\n", time_ms, out_len, nbytes);

	return out_len;
}

static int opus_encode_action(void *priv, recorder_action_t action, void *arg)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	opus_encoder_t *opus;
	int ret;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	opus = mencoder->encoder;
	if (!opus) {
		M_LOGE("opus encoder null !\n");
		return -1;
	}

	if (action == RECORDER_START) {
		int mem_size = opus_encoder_get_size(mencoder->channels);
		if (mem_size <= 0) {
			M_LOGE("get mem size failed %d!\n", mem_size);
			return -1;
		}

		M_LOGD("opus mem size %d\n", mem_size);
		opus->handler = snd_zalloc(mem_size, AFM_MAIN);
		if (!opus->handler) {
			M_LOGE("alloc opus mem failed !\n");
			return -1;
		}

		ret = opus_encoder_init(opus->handler, mencoder->rate,
				mencoder->channels, OPUS_APPLICATION_VOIP);
		if (ret != OPUS_OK) {
			snd_free(opus->handler);
			opus->handler = NULL;
			M_LOGE("opus init failed %d!\n", ret);
			return -1;
		}

		M_LOGD("rate %d channels %d bitrate %d\n",
			mencoder->rate,
			mencoder->channels, mencoder->bitrate);

		opus->rate = mencoder->rate;
		opus_encoder_ctl(opus->handler, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
		opus_encoder_ctl(opus->handler, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_NARROWBAND));
		opus_encoder_ctl(opus->handler, OPUS_SET_FORCE_CHANNELS(mencoder->channels));
		opus_encoder_ctl(opus->handler, OPUS_SET_BITRATE(mencoder->bitrate));
		opus_encoder_ctl(opus->handler, OPUS_SET_COMPLEXITY(1));
		opus_encoder_ctl(opus->handler, OPUS_SET_VBR(0));
		opus_encoder_ctl(opus->handler, OPUS_SET_PACKET_LOSS_PERC(0));
		opus_encoder_ctl(opus->handler, OPUS_SET_LSB_DEPTH(16));
		opus_encoder_ctl(opus->handler, OPUS_SET_INBAND_FEC(0));
		opus_encoder_ctl(opus->handler, OPUS_SET_DTX(0));

		M_LOGD("opus encoder init\n");
	} else if (action == RECORDER_STOP) {
		if (opus->in) {
			snd_free(opus->in);
			opus->in = NULL;
		}
		if (opus->out) {
			snd_free(opus->out);
			opus->out = NULL;
		}
		snd_free(opus->handler);
		M_LOGD("opus encoder free\n");
	}

	return 0;
}

int opus_encode_create(media_encoder_t *mencoder)
{
	opus_encoder_t *opus;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	opus = snd_zalloc(sizeof(opus_encoder_t), AFM_MAIN);
	if (!opus) {
		M_LOGE("alloc opus encoder failed !\n");
		return -1;
	}

	mencoder->encoder = opus;
	mencoder->encode  = opus_encode_process;
	mencoder->action  = opus_encode_action;

	M_LOGI("opus encoder create\n");
	return 0;
}

int opus_encode_release(media_encoder_t *mencoder)
{
	opus_encoder_t *opus;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	opus = mencoder->encoder;
	if (!opus) {
		M_LOGE("opus encoder null !\n");
		return -1;
	}

	snd_free(opus);
	mencoder->encoder = NULL;

	M_LOGI("opus encoder release\n");
	return 0;
}
