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
#include "uvoice_player.h"

#include "uvoice_common.h"
#include "uvoice_play.h"

#include "opensource/opus/source/include/opus.h"


#define OPUS_FRAME_LEN_MAX		1500
#define OPUS_FRAME_SAMPLES_MAX	1280

typedef struct {
	OpusDecoder *handler;
	int rate;
	int channels;
} opus_decoder_t;

static int opus_rate;
static int opus_channels;

static uint32_t char_to_int(uint8_t ch[4])
{
    return ((uint32_t)ch[0] << 24) | ((uint32_t)ch[1] << 16) |
		((uint32_t)ch[2] << 8) | (uint32_t)ch[3];
}

static bool opus_rate_valid(int rate)
{
	switch (rate) {
	case 8000:
	case 12000:
	case 16000:
	case 24000:
	case 48000:
		return true;
	}
	return false;
}

static bool opus_channels_valid(int channels)
{
	if (channels == 1 || channels == 2)
		return true;
	return false;
}

static int opus_decode_reset(void *priv)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	opus_decoder_t *opus = mdecoder->decoder;
	if (!opus) {
		M_LOGE("opus decoder null !\n");
		return -1;
	}


	M_LOGI("opus decoder reset\n");
	return 0;
}

static int opus_decode_process(void *priv, uint8_t *buffer, int nbytes)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	opus_decoder_t *opus;

	uint8_t *frame_ptr = buffer;
	int remaining_len = nbytes;
	int frame_len;

	int out_samples;
	short sample;
	int ret;
	int i;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	char *pcm_out = mdecoder->buffer_out;
	short *dec_out = mdecoder->buffer_out;

	opus = mdecoder->decoder;
	if (!opus) {
		M_LOGE("opus decoder null !\n");
		return -1;
	}

	while (1) {
		if (remaining_len <= 8) {
			mdecoder->unproc_size = remaining_len;
			break;
		}

		frame_len = char_to_int(frame_ptr);
		frame_ptr += 8;
		remaining_len -= 8;

		if (frame_len > remaining_len) {
			mdecoder->unproc_size = remaining_len + 8;
			break;
		}

		if (frame_len > OPUS_FRAME_LEN_MAX || frame_len <= 0) {
			M_LOGE("frame len %d invalid !\n", frame_len);
			return -1;
		}

		out_samples = opus_decode(opus->handler, frame_ptr,
			frame_len,
			mdecoder->buffer_out, OPUS_FRAME_SAMPLES_MAX, 0);

		frame_ptr += frame_len;
		remaining_len -= frame_len;

		if (out_samples <= 0) {
			M_LOGE("decoder error %d!\n", out_samples);
			continue;
		}

		if (!mdecoder->running) {
			if (mdecoder->message) {
				media_pcminfo_t pcm_info;
				memset(&pcm_info, 0, sizeof(pcm_info));
				pcm_info.rate = opus->rate;
				pcm_info.frames = out_samples;
				if (mdecoder->stere_enable)
					pcm_info.channels = opus->channels;
				else
					pcm_info.channels = 1;

				pcm_info.bits = 16;
				mdecoder->message(mdecoder->priv,
					PLAYER_MSG_PCM_INFO, &pcm_info);
			}
			mdecoder->running = 1;
		}

		if (out_samples > OPUS_FRAME_SAMPLES_MAX) {
			M_LOGE("out samples %d overrange !\n", out_samples);
			return -1;
		}

		if (out_samples * opus->channels * sizeof(short) >
				mdecoder->buffer_out_size) {
			M_LOGE("out size %d over buffer length !\n",
				out_samples * opus->channels * sizeof(short));
			return -1;
		}

		/*temp = 0;
		ret = opus_decoder_ctl(opus->handler, OPUS_GET_SAMPLE_RATE(&temp));
		if (ret) {
			M_LOGE("get sample rate failed %d!\n", ret);
		} else {
			M_LOGI("sample rate %d\n", temp);
		}*/

		for (i = 0; i < out_samples * opus->channels; i++) {
			sample = dec_out[i];
			pcm_out[2 * i] = sample & 0xff;
			pcm_out[2 * i + 1] = (sample >> 8) & 0xff;
			if (!mdecoder->stere_enable && opus->channels == 2)
				i++;
		}

		if (mdecoder->output(mdecoder->priv,
				mdecoder->buffer_out,
				out_samples * opus->channels * sizeof(short))) {
			M_LOGE("output failed !\n");
			return -1;
		}
	}

	return 0;
}

static int opus_decode_action(void *priv, player_action_t action, void *arg)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	opus_decoder_t *opus;
	int ret;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	opus = mdecoder->decoder;
	if (!opus) {
		M_LOGE("opus decoder null !\n");
		return -1;
	}

	if (action == PLAYER_CONFIGURE) {
		media_pcminfo_t *pcminfo = arg;
		if (!pcminfo) {
			M_LOGE("pcm info null !\n");
			return -1;
		}

		if (!opus_rate_valid(pcminfo->rate)) {
			M_LOGE("sample rate invalid !\n");
			return -1;
		}

		opus->rate = pcminfo->rate;
		opus_rate = opus->rate;

		if (!opus_channels_valid(pcminfo->channels)) {
			M_LOGE("channels invalid !\n");
			return -1;
		}
		opus->channels = pcminfo->channels;
		opus_channels = opus->channels;

		M_LOGI("rate %d channels %d\n", opus->rate, opus->channels);
	} else if (action == PLAYER_START ||
			action == PLAYER_RESUME || action == STREAM_MGR_START) {
		int error = -1;
		if (!opus_rate_valid(opus->rate)) {
			if (opus_rate_valid(opus_rate)) {
				opus->rate = opus_rate;
			} else {
				M_LOGE("no valid rate !\n");
				return -1;
			}
		}

		if (!opus_channels_valid(opus->channels)) {
			if (opus_channels_valid(opus_channels)) {
				opus->channels = opus_channels;
			} else {
				M_LOGE("no valid channels !\n");
				return -1;
			}
		}

		opus->handler = opus_decoder_create(opus->rate,
			opus->channels, &error);
		if (!opus->handler) {
			M_LOGE("create opus decoder failed !\n");
			return -1;
		}
		M_LOGI("opus decoder create, rate %d channels %d\n",
			opus->rate, opus->channels);
	} else if (action == PLAYER_STOP ||
			action == PLAYER_PAUSE || action == STREAM_MGR_STOP) {
		if (opus->handler) {
			opus_decoder_destroy(opus->handler);
			opus->handler = NULL;
			M_LOGI("opus decoder release\n");
		}
	}

	return 0;
}

int opus_decode_create(media_decoder_t *mdecoder)
{
	opus_decoder_t *opus;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	mdecoder->buffer_out_size =
		OPUS_FRAME_SAMPLES_MAX * sizeof(int16_t) * 2;
	M_LOGI("out buffer %u\n", mdecoder->buffer_out_size);
	mdecoder->buffer_out = snd_zalloc(
			mdecoder->buffer_out_size, AFM_EXTN);
	if (!mdecoder->buffer_out) {
		M_LOGE("alloc out buffer failed !\n");
		return -1;
	}
	mdecoder->input_size = 1024;

	opus = snd_zalloc(sizeof(opus_decoder_t), AFM_MAIN);
	if (!opus) {
		M_LOGE("alloc opus decoder failed !\n");
		return -1;
	}

	mdecoder->decoder = opus;
	mdecoder->decode  = opus_decode_process;
	mdecoder->action  = opus_decode_action;
	mdecoder->reset = opus_decode_reset;

	M_LOGI("opus decoder create\n");
	return 0;
}

int opus_decode_release(media_decoder_t *mdecoder)
{
	opus_decoder_t *opus;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	snd_free(mdecoder->buffer_out);
	mdecoder->buffer_out_size = 0;
	mdecoder->input_size = 0;

	opus = mdecoder->decoder;
	if (!opus) {
		M_LOGE("opus decoder null !\n");
		return -1;
	}

	snd_free(opus);
	mdecoder->decoder = NULL;

	M_LOGI("opus decoder release\n");
	return 0;
}

