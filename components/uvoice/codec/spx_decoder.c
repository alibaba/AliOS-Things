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

#include "opensource/speex/include/speex/speex_callbacks.h"


typedef struct {
	void *speex;
	SpeexBits bits;
	int frame_size;
	int bitwidth;
	int rate;
	int channels;
	int bitrate;
} spx_decoder_t;

static int spx_rate;
static int spx_channels;

static bool spx_rate_valid(int rate)
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

static bool spx_channels_valid(int channels)
{
	if (channels == 1 || channels == 2)
		return true;
	return false;
}

static int spx_decoder_reset(void *priv)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	spx_decoder_t *spx = mdecoder->decoder;
	if (!spx) {
		M_LOGE("spx decoder null !\n");
		return -1;
	}

	M_LOGD("speex decoder reset\n");
	return 0;
}

static int spx_decoder_process(void *priv, uint8_t *buffer, int nbytes)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	spx_decoder_t *spx = mdecoder->decoder;
	if (!spx) {
		M_LOGE("speex null !\n");
		return -1;
	}

	SpeexBits *bits = &spx->bits;

	spx_int16_t *out_buffer = (spx_int16_t *)mdecoder->buffer_out;
	int offset = 0;
	int frame_size;
	int ret;
	int i;

	while (1) {
		frame_size = *(int *)(buffer + offset);
		offset += 4;
		if (frame_size > nbytes - offset) {
			mdecoder->unproc_size = nbytes - offset + 4;
			break;
		}
		snd_memcpy(bits->chars, buffer + offset, frame_size);

		bits->nbBits = frame_size << 3;
		speex_bits_rewind(&spx->bits);

		ret = speex_decode_int(spx->speex, bits, out_buffer);

		offset += frame_size;

		if (!mdecoder->running) {
			if (mdecoder->message) {
				spx_int32_t temp = 0;
				speex_decoder_ctl(spx->speex, SPEEX_GET_BITRATE, &temp);
				spx->bitrate = temp;
				//M_LOGD("rate %d, channels %d, bitrate %d, frame_size %d, bits %d\n",
				//		spx->rate,
				//		spx->channels,
				//		spx->bitrate,
				//		spx->frame_size, 16);
				media_pcminfo_t pcm_info;
				memset(&pcm_info, 0, sizeof(pcm_info));
				pcm_info.rate = spx->rate;
				pcm_info.frames = spx->frame_size / spx->channels;
				if (mdecoder->stere_enable)
					pcm_info.channels = spx->channels;
				else
					pcm_info.channels = 1;
				pcm_info.bits = 16;
				mdecoder->message(mdecoder->priv,
					PLAYER_MSG_PCM_INFO, &pcm_info);

				media_info_t media_info;
				memset(&media_info, 0, sizeof(media_info_t));
				media_info.bitrate = spx->bitrate;
				mdecoder->message(mdecoder->priv,
					PLAYER_MSG_MEDIA_INFO, &media_info);
			}
			mdecoder->running = 1;
		}

		if (mdecoder->output(mdecoder->priv, mdecoder->buffer_out,
				spx->frame_size * sizeof(short))) {
			M_LOGE("output failed !\n");
			return -1;
		}
	}

	speex_bits_reset(&spx->bits);

__exit:
	return 0;
}

static int spx_decoder_action(void *priv,
		player_action_t action, void *arg)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	spx_decoder_t *spx;
	spx_int32_t temp;
	int ret;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	spx = mdecoder->decoder;
	if (!spx) {
		M_LOGE("spx decoder null !\n");
		return -1;
	}

	if (action == PLAYER_CONFIGURE) {
		media_pcminfo_t *pcminfo = arg;
		if (!pcminfo) {
			M_LOGE("pcm info null !\n");
			return -1;
		}

		if (!spx_rate_valid(pcminfo->rate)) {
			M_LOGE("sample rate invalid !\n");
			return -1;
		}

		spx->rate = pcminfo->rate;
		spx_rate = spx->rate;
		temp = spx->rate;
		speex_decoder_ctl(spx->speex, SPEEX_SET_SAMPLING_RATE, &temp);

		if (!spx_channels_valid(pcminfo->channels)) {
			M_LOGE("channels invalid !\n");
			return -1;
		}
		spx->channels = pcminfo->channels;
		spx_channels = spx->channels;

		M_LOGD("rate %d channels %d\n", spx->rate, spx->channels);
	} else if (action == PLAYER_START ||
			action == PLAYER_RESUME || action == STREAM_MGR_START) {
		int error = -1;
		if (!spx_rate_valid(spx->rate)) {
			if (spx_rate_valid(spx_rate)) {
				spx->rate = spx_rate;
			} else {
				M_LOGE("no valid rate !\n");
				return -1;
			}
		}

		if (!spx_channels_valid(spx->channels)) {
			if (spx_channels_valid(spx_channels)) {
				spx->channels = spx_channels;
			} else {
				M_LOGE("no valid channels !\n");
				return -1;
			}
		}

		M_LOGD("rate %d channels %d\n", spx->rate, spx->channels);
	}

	return 0;
}

int spx_decoder_create(media_decoder_t *mdecoder)
{
	spx_decoder_t *spx;
	spx_int32_t temp;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	spx = snd_zalloc(sizeof(spx_decoder_t), AFM_MAIN);
	if (!spx) {
		M_LOGE("alloc speex decoder failed !\n");
		return -1;
	}

	spx->speex = speex_decoder_init(
		speex_lib_get_mode(SPEEX_MODEID_NB));
	if (!spx->speex) {
		M_LOGE("init speex failed !\n");
		snd_free(spx);
		return -1;
	}

	speex_bits_init(&spx->bits);

	temp = 1;
	speex_decoder_ctl(spx->speex, SPEEX_SET_HIGHPASS, &temp);
	temp = 1;
	speex_decoder_ctl(spx->speex, SPEEX_SET_ENH, &temp);
	speex_decoder_ctl(spx->speex, SPEEX_GET_FRAME_SIZE, &temp);
	spx->frame_size = temp;
	M_LOGD("frame size %d\n", spx->frame_size);

	mdecoder->buffer_out_size = spx->frame_size * sizeof(short);
	mdecoder->buffer_out = snd_zalloc(mdecoder->buffer_out_size, AFM_EXTN);
	if (!mdecoder->buffer_out) {
		M_LOGE("alloc out buffer failed !\n");
		speex_decoder_destroy(spx->speex);
		speex_bits_destroy(&spx->bits);
		snd_free(spx);
		return -1;
	}

	mdecoder->input_size = spx->bits.buf_size;

	mdecoder->decoder = spx;
	mdecoder->decode = spx_decoder_process;
	mdecoder->action = spx_decoder_action;
	mdecoder->reset = spx_decoder_reset;

	M_LOGD("speex decoder create\n");
	return 0;
}

int spx_decoder_release(media_decoder_t *mdecoder)
{
	spx_decoder_t *spx;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	spx = mdecoder->decoder;
	if (!spx) {
		M_LOGE("speex decoder null !\n");
		return -1;
	}

	speex_decoder_destroy(spx->speex);
	speex_bits_destroy(&spx->bits);
	snd_free(spx);
	mdecoder->decoder = NULL;

	M_LOGD("speex decoder release\n");
	return 0;
}
