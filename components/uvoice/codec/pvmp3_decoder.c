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

#include "uvoice_os.h"
#include "uvoice_common.h"
#include "uvoice_play.h"

#include "pvmp3decoder_api.h"

#define NO_DECODING_ERROR 0
#define FRAME_HEADER_MASK		0xfffe0c00
#define FRAME_HEADER_SIZE		4

#define HEADER_PACK(ptr)		\
	((ptr)[0] << 24 | (ptr)[1] << 16 | (ptr)[2] << 8 | (ptr)[3])

enum {
	PVMP3_CONTINUE = 0,
	PVMP3_BREAK,
	PVMP3_STOP,
};

typedef struct {
	void *handler;
	uint8_t *frame_ptr;
	int32_t pos;
	int32_t proc_sum;
	int32_t rate;
	int32_t channels;
	int32_t bitrate;
	int32_t frame_size;
	int32_t frame_samples;
	int32_t skip_id3_size;
	uint32_t fixed_header;
	tPVMP3DecoderExternal *config;
} pvmp3_decoder_t;


/*
 * Taken from PVMP3 src
 */
static int pvmp3_parse_header(pvmp3_decoder_t *decoder, uint32_t header)
{
	if ((header & 0xffe00000) != 0xffe00000)
		return -1;

	uint8_t version = (uint8_t)((header >> 19) & 3);
	if (version == 0x01)
		return -1;

	uint8_t layer = (uint8_t)((header >> 17) & 3);
	if (layer == 0x00)
		return -1;

	uint8_t bitrate_idx = (uint8_t)((header >> 12) & 0x0f);
	if (bitrate_idx == 0 || bitrate_idx == 0x0f)
		return -1;

	uint8_t rate_idx = (uint8_t)((header >> 10) & 3);
	if (rate_idx == 3)
		return -1;

	static const int rate_v1[] = { 44100, 48000, 32000 };
	int sample_rate = rate_v1[rate_idx];
	if (version == 2 /* V2 */)
		sample_rate /= 2;
	else if (version == 0 /* V2.5 */)
		sample_rate /= 4;

	uint8_t padding = (header >> 9) & 1;

	if (layer == 3) {
		/* layer I */
		static const int bitrate_v1[] = {
			32,	64, 96, 128, 160, 192, 224, 256,
			288, 320, 352, 384, 416, 448
		};

		static const int bitrate_v2[] = {
			32, 48, 56, 64, 80, 96, 112, 128,
			144, 160, 176, 192, 224, 256
		};

		decoder->bitrate = (version == 3) ?
			bitrate_v1[bitrate_idx - 1] : bitrate_v2[bitrate_idx - 1];

		decoder->frame_size = (12000 * decoder->bitrate / sample_rate + padding) * 4;
		//M_LOGD("layer1 bitrate %d rate %d padding %u frame_size %d pos %d\n",
		//	decoder->bitrate,
		//	sample_rate, padding, decoder->frame_size, decoder->pos);
		decoder->frame_samples = 384;
	} else {
		/* layer II or III */
		static const int bitrate_v1_l2[] = {
			32, 48, 56, 64, 80, 96, 112, 128,
			160, 192, 224, 256, 320, 384
		};

		static const int bitrate_v1_l3[] = {
			32, 40, 48, 56, 64, 80, 96, 112,
			128, 160, 192, 224, 256, 320
		};

		static const int bitrate_v2[] = {
			8, 16, 24, 32, 40, 48, 56, 64,
			80, 96, 112, 128, 144, 160
		};

		if (version == 3) {
			/* V1 */
			decoder->bitrate = (layer == 2 /* L2 */) ? bitrate_v1_l2[bitrate_idx - 1]
					: bitrate_v1_l3[bitrate_idx - 1];
			decoder->frame_samples = 1152;
		} else {
			/* V2 (or 2.5) */
            decoder->bitrate = bitrate_v2[bitrate_idx - 1];
            decoder->frame_samples = (layer == 1 /* L3 */) ? 576 : 1152;
        }

		if (version == 3) {
			/* V1 */
			decoder->frame_size = 144000 * decoder->bitrate / sample_rate + padding;
			//M_LOGD("v1 bitrate %d rate %d padding %u frame_size %d pos %d\n",
			//	decoder->bitrate,
			//	sample_rate,
			//	padding, decoder->frame_size, decoder->pos);
		} else {
			/* V2 or V2.5 */
            int tmp = (layer == 1) ? 72000 : 144000;
			decoder->frame_size = tmp * decoder->bitrate / sample_rate + padding;
			//M_LOGD("v2 bitrate %d rate %d padding %u frame_size %d pos %d\n",
			//	decoder->bitrate,
			//	sample_rate,
			//	padding, decoder->frame_size, decoder->pos);
		}
	}

	decoder->rate = sample_rate;
	decoder->channels = (((header >> 6) & 3) == 3) ? 1 : 2;
	return 0;
}

static int pvmp3_frame_locate(media_decoder_t *mdecoder,
		uint8_t *buffer, int len)
{
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return PVMP3_STOP;
	}

	pvmp3_decoder_t *decoder = mdecoder->decoder;
	int skip_size;

	if (decoder->proc_sum == 0) {
		uint8_t ID3_desc[10];
		int32_t ID3_size;

		if (len - decoder->pos < sizeof(ID3_desc)) {
			mdecoder->unproc_size = len - decoder->pos;
			M_LOGW("buffer too short !\n");
			return PVMP3_BREAK;
		}

		memcpy(ID3_desc, buffer + decoder->pos, sizeof(ID3_desc));

		if (!memcmp("ID3", ID3_desc, 3)) {
			ID3_size = ((ID3_desc[6] & 0x7f) << 21)
	            	| ((ID3_desc[7] & 0x7f) << 14)
	            	| ((ID3_desc[8] & 0x7f) << 7)
	            	| (ID3_desc[9] & 0x7f);

			ID3_size += 10;

			decoder->skip_id3_size = ID3_size;

			//M_LOGD("ID3 size %d pos %d len %d\n",
			//	decoder->skip_id3_size, decoder->pos, len);

			skip_size = MIN(decoder->skip_id3_size, (len - decoder->pos));
			decoder->skip_id3_size -= skip_size;
			decoder->proc_sum += skip_size;
			decoder->pos += skip_size;
			if (decoder->pos >= len) {
				mdecoder->unproc_size = 0;
				return PVMP3_BREAK;
			}
		}
	}

	if (decoder->skip_id3_size > 0) {
		skip_size = MIN(decoder->skip_id3_size, (len - decoder->pos));
		decoder->skip_id3_size -= skip_size;
		decoder->pos += skip_size;
		decoder->proc_sum += skip_size;
		if (decoder->pos >= len) {
			//M_LOGD("skip_size %d skip_id3_size %d pos %d\n",
			//	skip_size, decoder->skip_id3_size, decoder->pos);
			mdecoder->unproc_size = 0;
			return PVMP3_BREAK;
		} else {
			//M_LOGD("skip id3 cplt, skip_id3_size %d pos %d proc_sum %d\n",
			//	decoder->skip_id3_size,
			//	decoder->pos, decoder->proc_sum);
		}
	}

	int frame_pos = decoder->pos;
	int frame_size_bak = 0;
	int next_frame_pos = 0;
	uint32_t header = 0;
	uint32_t next_header = 0;

	char frame_found = 0;
	char assert_cnt = len >= 1045 ? len/1045 : 1;
	char i;

	do {
		if (frame_pos + FRAME_HEADER_SIZE > len) {
			mdecoder->unproc_size = len - frame_pos;
			decoder->proc_sum += frame_pos - decoder->pos;
			//M_LOGD("too less: frame_pos %d frame_size %d pos %d proc_sum %d unproc_size %u\n",
			//	frame_pos,
			//	decoder->frame_size,
			//	decoder->pos, decoder->proc_sum, mdecoder->unproc_size);
			return PVMP3_BREAK;
		}

        header = HEADER_PACK(buffer + frame_pos);

		if (decoder->fixed_header != 0 &&
			(header & FRAME_HEADER_MASK) != (decoder->fixed_header & FRAME_HEADER_MASK)) {
			frame_pos++;
			continue;
		}

		if (pvmp3_parse_header(decoder, header)) {
			frame_pos++;
			continue;
		}

		//M_LOGD("frame maybe: frame_pos %d frame_size %d pos %d g_pos %d unproc_size %u\n",
		//		frame_pos,
		//		decoder->frame_size,
		//		decoder->pos, decoder->proc_sum, mdecoder->unproc_size);

		/* Assert frame validation */
		frame_size_bak = decoder->frame_size;
		frame_found = 1;
		next_frame_pos = frame_pos;

		for (i = 0; i < assert_cnt; i++) {
			next_frame_pos += decoder->frame_size;
			//M_LOGD("i %d frame_pos %d next_frame_pos %d\n",
			//		i, frame_pos, next_frame_pos);
			if (next_frame_pos + FRAME_HEADER_SIZE > len) {
				decoder->proc_sum += frame_pos - decoder->pos;
				mdecoder->unproc_size = len - frame_pos;
				//M_LOGD("require more: frame_pos %d frame_size %d pos %d proc_sum %d unproc_size %u\n",
				//		frame_pos,
				//		decoder->frame_size,
				//		decoder->pos, decoder->proc_sum, mdecoder->unproc_size);
				return PVMP3_BREAK;
			}

			next_header = HEADER_PACK(buffer + next_frame_pos);
			if (decoder->fixed_header != 0 &&
				(next_header & FRAME_HEADER_MASK) != (decoder->fixed_header & FRAME_HEADER_MASK)) {
				frame_pos++;
				frame_found = 0;
				break;
			}

			if (pvmp3_parse_header(decoder, next_header)) {
				frame_pos++;
				frame_found = 0;
				break;
			}
		}

		if (!frame_found)
			continue;

		if (decoder->fixed_header == 0) {
			decoder->fixed_header = header;
			//M_LOGD("fixed header 0x%x\n", decoder->fixed_header);
		}

		decoder->frame_size = frame_size_bak;
		decoder->frame_ptr = buffer + frame_pos;
		decoder->proc_sum += frame_pos + decoder->frame_size - decoder->pos;
		decoder->pos = frame_pos + decoder->frame_size;
		mdecoder->unproc_size = len - decoder->pos;

		//M_LOGD("frame found: frame_pos %d next_frame_pos %d frame_size %d pos %d g_pos %d unproc_size %d\n",
		//		frame_pos,
		//		next_frame_pos,
		//		decoder->frame_size,
		//		decoder->pos, decoder->proc_sum, mdecoder->unproc_size);
    } while (!frame_found);

    return PVMP3_CONTINUE;
}

static int pvmp3_decoder_reset(void *priv)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	pvmp3_decoder_t *decoder = mdecoder->decoder;
	if (!decoder) {
		M_LOGE("decoder null !\n");
		return -1;
	}

	decoder->bitrate = 0;
	decoder->channels = 0;
	decoder->rate = 0;
	decoder->pos = 0;
	decoder->proc_sum = 0;
	decoder->skip_id3_size = 0;
	decoder->frame_ptr = NULL;
	decoder->fixed_header = 0;
	decoder->frame_samples = 0;
	decoder->frame_size = 0;

	pvmp3_resetDecoder(decoder->handler);

	M_LOGD("pvmp3 reset\n");
	return 0;
}

static int pvmp3_decoder_process(void *priv, uint8_t *buffer, int nbytes)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	pvmp3_decoder_t *decoder = mdecoder->decoder;
	if (!decoder) {
		M_LOGE("decoder null !\n");
		return -1;
	}

	tPVMP3DecoderExternal *config = decoder->config;
	int16_t *out_buffer = (int16_t *)mdecoder->buffer_out;
	int out_samples;
	int i;
	int ret;

	decoder->pos = 0;

	if (nbytes != mdecoder->input_size)
		M_LOGD("tail buff %d\n", nbytes);

	while (1) {
		ret = pvmp3_frame_locate(mdecoder, buffer, nbytes);
		if (ret == PVMP3_BREAK) {
			//M_LOGD("request more data, unproc size %u\n",
			//	mdecoder->unproc_size);
			break;
		} else if (ret == PVMP3_STOP) {
			M_LOGE("decoder error !\n");
			return -1;
		}

		config->inputBufferCurrentLength = nbytes - (decoder->frame_ptr - buffer);
		config->pInputBuffer = decoder->frame_ptr;
		config->inputBufferMaxLength = nbytes - (decoder->frame_ptr - buffer);
		config->inputBufferUsedLength = 0;
		config->pOutputBuffer = (int16_t *)mdecoder->buffer_out;
		config->outputFrameSize = mdecoder->buffer_out_size / sizeof(int16_t);

		ret = pvmp3_framedecoder(config, decoder->handler);
		if (ret != NO_DECODING_ERROR)
			continue;

		if (!mdecoder->running) {
			//M_LOGD("rate %d channels %d bitrate %d frame_size %d\n",
			//		config->samplingRate,
			//		config->num_channels,
			//		config->bitRate, config->outputFrameSize);
			media_pcminfo_t pcm_info;
			memset(&pcm_info, 0, sizeof(media_pcminfo_t));
			pcm_info.rate = config->samplingRate;
			pcm_info.frames = config->outputFrameSize / config->num_channels;
			if (mdecoder->stere_enable)
				pcm_info.channels = config->num_channels;
			else
				pcm_info.channels = 1;
			pcm_info.bits = 16;
			mdecoder->message(mdecoder->priv,
				PLAYER_MSG_PCM_INFO, &pcm_info);

			media_info_t media_info;
			memset(&media_info, 0, sizeof(media_info_t));
			media_info.bitrate = config->bitRate * 1000;
			mdecoder->message(mdecoder->priv,
				PLAYER_MSG_MEDIA_INFO, &media_info);
			mdecoder->running = 1;
		}

		out_samples = config->outputFrameSize;
		if (out_samples * sizeof(int16_t) > mdecoder->buffer_out_size) {
			M_LOGE("out samples %u overrange !\n", out_samples);
			return -1;
		}

		if (config->num_channels == 2 && !mdecoder->stere_enable) {
			out_samples = config->outputFrameSize / 2;
			for (i = 0; i < out_samples; i++)
				out_buffer[i] = out_buffer[2 * i];
		}

		if (mdecoder->output(mdecoder->priv,
				(uint8_t *)out_buffer, out_samples * sizeof(int16_t))) {
			M_LOGE("output failed !\n");
			return -1;
		}
	}

	return 0;
}

static int pvmp3_decoder_action(void *priv,
		player_action_t action, void *arg)
{
	return 0;
}

int pvmp3_decoder_create(media_decoder_t *mdecoder)
{
	pvmp3_decoder_t *decoder;
	int mem_size;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	decoder = snd_zalloc(
			sizeof(pvmp3_decoder_t), AFM_MAIN);
	if (!decoder) {
		M_LOGE("alloc pvmp3 decoder failed !\n");
		return -1;
	}

	decoder->config = snd_zalloc(
			sizeof(tPVMP3DecoderExternal), AFM_MAIN);
	if (!decoder->config) {
		M_LOGE("alloc decoder config failed !\n");
		snd_free(decoder);
		return -1;
	}

	decoder->config->equalizerType = flat;
	decoder->config->crcEnabled = false;

    mem_size = pvmp3_decoderMemRequirements();

    decoder->handler = snd_zalloc(mem_size, AFM_MAIN);
	if (!decoder->handler) {
		M_LOGE("alloc decoder memory failed !\n");
		snd_free(decoder->config);
		snd_free(decoder);
		return -1;
	}

	pvmp3_InitDecoder(decoder->config, decoder->handler);

	mdecoder->decode = pvmp3_decoder_process;
	mdecoder->action = pvmp3_decoder_action;
	mdecoder->reset = pvmp3_decoder_reset;
	mdecoder->decoder = decoder;

	M_LOGD("pvmp3 create\n");
	return 0;
}

int pvmp3_decoder_release(media_decoder_t *mdecoder)
{
	pvmp3_decoder_t *decoder;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	decoder = mdecoder->decoder;
	if (!decoder) {
		M_LOGE("decoder null !\n");
		return -1;
	}

	snd_free(decoder->handler);
	snd_free(decoder->config);
	snd_free(decoder);
	mdecoder->decoder = NULL;
	M_LOGD("pvmp3 release\n");
	return 0;
}

