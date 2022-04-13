/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "uvoice_types.h"
#include "uvoice_player.h"
#include "uvoice_recorder.h"

#include "uvoice_os.h"
#include "uvoice_common.h"
#include "uvoice_play.h"
#include "uvoice_record.h"
#include "uvoice_wave.h"


typedef struct {
	int rate;
	int bits;
	int channels;
	uint16_t audio_format;
} wave_decoder_t;

static int wave_header_init(struct wav_header *header, int rate,
	int channels, int bits)
{
	if (!header) {
		M_LOGE("header null !\n");
		return -1;
	}

	header->riff_id = ID_RIFF;
	header->riff_fmt = ID_WAVE;
	header->fmt_id = ID_FMT;
	header->fmt_sz = 16;
	header->audio_format = WAVE_FMT_PCM;
	header->num_channels = channels;
	header->sample_rate = rate;
	header->bits_per_sample = bits;
	header->byte_rate = (header->bits_per_sample / 8) * channels * rate;
	header->block_align = channels * (header->bits_per_sample / 8);
	header->data_id = ID_DATA;
	return 0;
}

static int wave_info_parse(media_decoder_t *mdecoder, uint8_t *buffer,
	int nbytes)
{
	struct riff_wave_header riff_wave_header;
	struct chunk_header chunk_header;
	struct chunk_fmt chunk_fmt;
	int more_chunks = 1;
	int remaining_bytes = nbytes;
	uint8_t *ptr = buffer;
	wave_decoder_t *wave;

	media_pcminfo_t pcm_info;
	media_info_t media_info;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	if (!ptr) {
		M_LOGE("buffer null !\n");
		return -1;
	}

	wave = mdecoder->decoder;
	if (!wave) {
		M_LOGE("wave decoder null !\n");
		return -1;
	}

	if (remaining_bytes >= sizeof(struct riff_wave_header)) {
		memcpy(&riff_wave_header,
			ptr, sizeof(struct riff_wave_header));
		remaining_bytes -= sizeof(struct riff_wave_header);
		ptr += sizeof(struct riff_wave_header);
	} else {
		M_LOGE("parse riff wave header failed !\n");
		return -1;
	}

	if ((riff_wave_header.riff_id != ID_RIFF) ||
		(riff_wave_header.wave_id != ID_WAVE)) {
		M_LOGE("not riff/wave file !\n");
		return -1;
	}

	memset(&chunk_fmt, 0, sizeof(struct chunk_fmt));

	do {
		if (remaining_bytes >= sizeof(struct chunk_header)) {
			memcpy(&chunk_header, ptr, sizeof(struct chunk_header));
			remaining_bytes -= sizeof(struct chunk_header);
			ptr += sizeof(struct chunk_header);
		} else {
			M_LOGE("parse chunk header failed !\n");
			return -1;
		}

		switch (chunk_header.id) {
		case ID_FMT:
			if (remaining_bytes >= sizeof(struct chunk_fmt)) {
				memcpy(&chunk_fmt, ptr, sizeof(struct chunk_fmt));
				remaining_bytes -= sizeof(struct chunk_fmt);
				ptr += sizeof(struct chunk_fmt);
			} else {
				M_LOGE("parse chunk fmt failed !\n");
				return -1;
			}

			if (chunk_header.sz > sizeof(struct chunk_fmt)) {
				remaining_bytes -= (chunk_header.sz - sizeof(chunk_fmt));
				ptr += chunk_header.sz - sizeof(chunk_fmt);
			}

			M_LOGD("sample_rate %u\n", chunk_fmt.sample_rate);
			M_LOGD("channels %u\n", chunk_fmt.num_channels);
			M_LOGD("byte_rate %u\n", chunk_fmt.byte_rate);
			M_LOGD("audio_format %u\n", chunk_fmt.audio_format);
			M_LOGD("bits_per_sample %u\n", chunk_fmt.bits_per_sample);
			M_LOGD("block_align %u\n", chunk_fmt.block_align);
			break;
		case ID_DATA:
			more_chunks = 0;
			break;
		default:
			remaining_bytes -= chunk_header.sz;
			ptr += chunk_header.sz;
		}
	} while (more_chunks);

	memset(&pcm_info, 0, sizeof(pcm_info));
	pcm_info.rate = chunk_fmt.sample_rate;
	if (mdecoder->stere_enable)
		pcm_info.channels = chunk_fmt.num_channels;
	else
		pcm_info.channels = 1;
	pcm_info.bits = chunk_fmt.bits_per_sample;
	pcm_info.frames = mdecoder->input_size /
		(pcm_info.channels * (pcm_info.bits >> 3));
	wave->rate = chunk_fmt.sample_rate;
	wave->channels = chunk_fmt.num_channels;
	wave->bits = chunk_fmt.bits_per_sample;
	wave->audio_format = chunk_fmt.audio_format;

	mdecoder->message(mdecoder->priv,
			PLAYER_MSG_PCM_INFO, &pcm_info);

	memset(&media_info, 0, sizeof(media_info));
	media_info.bitrate = chunk_fmt.byte_rate * 8;

	mdecoder->message(mdecoder->priv,
			PLAYER_MSG_MEDIA_INFO, &media_info);

	if (remaining_bytes >= 0)
		memset(buffer, 0, nbytes - remaining_bytes);
	M_LOGD("remaining_bytes %d\n", remaining_bytes);

	return remaining_bytes;
}

static int wave_decode_process(void *priv, uint8_t *buffer,
	int nbytes)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	wave_decoder_t *wave;
	short *out_buffer = (short *)buffer;
	int out_samples;
	int i;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	wave = mdecoder->decoder;
	if (!wave) {
		M_LOGE("wave decoder null !\n");
		return -1;
	}

	if (!mdecoder->running) {
		if (wave_info_parse(mdecoder, buffer, nbytes) < 0) {
			M_LOGE("parse wave info failed !\n");
			return -1;
		}
		mdecoder->running = 1;
	}

	if (wave->channels == 2 && !mdecoder->stere_enable) {
		out_samples = (nbytes / (wave->bits >> 3)) / 2;
		for (i = 0; i < out_samples; i++)
			out_buffer[i] = out_buffer[2 * i];
		nbytes /= 2;
	}

	if (mdecoder->output(mdecoder->priv, buffer, nbytes)) {
		M_LOGE("output failed !\n");
		return -1;
	}

	return 0;
}

static int wave_decode_action(void *priv, player_action_t action,
	void *arg)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	wave_decoder_t *wave;
	static int wave_bits = 0;
	static int wave_channels = 0;
	static int wave_sample_rate = 0;
	static uint16_t audio_format = 0;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	wave = mdecoder->decoder;
	if (!wave) {
		M_LOGE("wave decoder null !\n");
		return -1;
	}

	switch (action) {
	case PLAYER_PAUSE:
		audio_format = wave->audio_format;
		wave_sample_rate = wave->rate;
		wave_channels = wave->channels;
		wave_bits = wave->bits;
		break;
	case PLAYER_RESUME:
		wave->audio_format = audio_format;
		wave->rate = wave_sample_rate;
		wave->channels = wave_channels;
		wave->bits = wave_bits;
		mdecoder->running = 1;
		break;
	default:
		break;
	}

	return 0;
}

int wave_decoder_create(media_decoder_t *mdecoder)
{
	wave_decoder_t *wave;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	wave = snd_zalloc(sizeof(wave_decoder_t), AFM_EXTN);
	if (!wave) {
		M_LOGE("alloc wave decoder failed !\n");
		return -1;
	}

	mdecoder->decode = wave_decode_process;
	mdecoder->action = wave_decode_action;
	mdecoder->decoder = wave;
	mdecoder->input_size = 2048;

	M_LOGD("wav decoder create\n");
	return 0;
}

int wave_decoder_release(media_decoder_t *mdecoder)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	snd_free(mdecoder->decoder);
	mdecoder->decoder = NULL;
	mdecoder->decode = NULL;
	mdecoder->action = NULL;

	M_LOGD("wav decoder release\n");
	return 0;
}

static int wave_encode_process(void *priv, uint8_t *buffer, int nbytes)
{
	return nbytes;
}

static int wave_encode_action(void *priv,
	recorder_action_t action, void *arg)
{
	return 0;
}

static int wave_encode_header_gen(void *priv, void *arg)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	media_pcminfo_t *pcminfo;
	struct wav_header *header;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	pcminfo = arg;
	if (!pcminfo) {
		M_LOGE("pcminfo null !\n");
		return -1;
	}

	mencoder->header_size = sizeof(struct wav_header);

	header = snd_zalloc(mencoder->header_size, AFM_EXTN);
	if (!header) {
		M_LOGE("alloc header failed !\n");
		return -1;
	}

	M_LOGD("rate %d channels %d bits %d\n",
		pcminfo->rate,
		pcminfo->channels, pcminfo->bits);

	if (wave_header_init(header, pcminfo->rate,
		pcminfo->channels, pcminfo->bits)) {
		M_LOGE("init wave header failed !\n");
		snd_free(header);
		mencoder->header_size = 0;
		return -1;
	}

	mencoder->header_cplt = 0;
	mencoder->header = header;
	return 0;
}

static int wave_encode_header_update(void *priv, int size)
{
	media_encoder_t *mencoder = (media_encoder_t *)priv;
	struct wav_header *header;

	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	header = mencoder->header;
	if (!header) {
		M_LOGE("header null !\n");
		return -1;
	}

	header->riff_sz = size - 8;
	header->data_sz = size - sizeof(struct wav_header);
	mencoder->header_cplt = 1;
	return 0;
}

int wave_encoder_create(media_encoder_t *mencoder)
{
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	mencoder->encode = wave_encode_process;
	mencoder->action = wave_encode_action;
	mencoder->header_gen = wave_encode_header_gen;
	mencoder->header_update = wave_encode_header_update;

	M_LOGD("wav encoder create\n");
	return 0;
}

int wave_encoder_release(media_encoder_t *mencoder)
{
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return -1;
	}

	mencoder->encode = NULL;
	mencoder->action = NULL;
	mencoder->header_gen = NULL;

	M_LOGD("wav encoder release\n");
	return 0;
}

