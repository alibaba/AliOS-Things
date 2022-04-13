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

#include "opensource/amr/sp_dec.h"


#define AMR_MAGIC_NUMBER		"#!AMR\n"

typedef struct {
	int frame_type; /* 0: IF1, 1: IF2 */
	void *amrdec;
} amr_decoder_t;

static int amr_decoder_reset(void *priv)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	amr_decoder_t *amr;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	amr = mdecoder->decoder;
	if (!amr) {
		M_LOGE("amr decoder null !\n");
		return -1;
	}

	Decoder_Interface_reset(amr->amrdec);
	M_LOGD("amr decoder reset\n");
	return 0;
}

static int amr_decoder_process(void *priv, const uint8_t *buffer, int nbytes)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	amr_decoder_t *amr;
	const uint8_t *readptr = buffer;
	uint8_t frame_header;
	int rem_size = nbytes;
	int frame_size;
	short *out_buffer;
	int ret;

	enum Mode dec_mode;
	short block_size[16] = {
		12,	13,	15,	17,	19,	20,	26,	31,
		5,	0,	0,	0,	0,	0,	0,	0
	};

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	amr = mdecoder->decoder;
	if (!amr) {
		M_LOGE("amr decoder null !\n");
		return -1;
	}

	out_buffer = mdecoder->buffer_out;

	if (!mdecoder->running) {
		if (memcmp(buffer, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER))) {
			M_LOGE("amr magic number not found !\n");
			return -1;
		}
		readptr += strlen(AMR_MAGIC_NUMBER);
		rem_size -= strlen(AMR_MAGIC_NUMBER);

		media_pcminfo_t pcm_info;
		memset(&pcm_info, 0, sizeof(pcm_info));
		pcm_info.rate = 8000;
		pcm_info.frames = 160;
		pcm_info.channels = 1;
		pcm_info.bits = 16;
		mdecoder->message(mdecoder->priv,
			PLAYER_MSG_PCM_INFO, &pcm_info);

		media_info_t media_info;
		memset(&media_info, 0, sizeof(media_info_t));
		media_info.bitrate = 12000;
		mdecoder->message(mdecoder->priv,
			PLAYER_MSG_MEDIA_INFO, &media_info);
		mdecoder->running = 1;
	}

	while (1) {
		frame_header = readptr[0];
		dec_mode = (frame_header >> 3) & 0xf;
		rem_size--;
		frame_size = block_size[dec_mode];
		if (rem_size < frame_size) {
			mdecoder->unproc_size = rem_size + 1;
			break;
		}

		if (frame_header & 0x04) {
			//M_LOGD("frame_size %d rem_size %d\n", frame_size, rem_size);
			Decoder_Interface_Decode(amr->amrdec, readptr, out_buffer, 0);

			if (mdecoder->output(mdecoder->priv,
				out_buffer, mdecoder->buffer_out_size)) {
				M_LOGE("output failed !\n");
				return -1;
			}
		} else {
			M_LOGW("bad frame\n");
		}

		readptr += frame_size + 1;
		rem_size -= frame_size;
		if (rem_size <= 0) {
			mdecoder->unproc_size = 0;
			break;
		}
	}

	return 0;
}

static int amr_decoder_action(void *priv, player_action_t action, void *arg)
{
	return 0;
}

int amr_decoder_create(media_decoder_t *mdecoder)
{
	amr_decoder_t *amr;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	amr = snd_zalloc(sizeof(amr_decoder_t), AFM_MAIN);
	if (!amr) {
		M_LOGE("alloc amr decoder failed !\n");
		return -1;
	}

	amr->amrdec = Decoder_Interface_init();
	if (!amr->amrdec) {
		M_LOGE("init amrdec failed !\n");
		snd_free(amr);
		return -1;
	}

	mdecoder->buffer_out_size = sizeof(short) * 160;
	mdecoder->buffer_out = snd_zalloc(mdecoder->buffer_out_size, AFM_MAIN);
	if (!mdecoder->buffer_out) {
		M_LOGE("alloc out buffer failed !\n");
		snd_free(amr);
		return -1;
	}

	mdecoder->input_size = 128;

	mdecoder->decode = amr_decoder_process;
	mdecoder->action = amr_decoder_action;
    mdecoder->reset = amr_decoder_reset;
    mdecoder->decoder = amr;

	M_LOGD("amr decoder create\n");
	return 0;
}

int amr_decoder_release(media_decoder_t *mdecoder)
{
	amr_decoder_t *amr;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	if (mdecoder->buffer_out) {
		snd_free(mdecoder->buffer_out);
		mdecoder->buffer_out = NULL;
		mdecoder->buffer_out_size = 0;
	}

    amr = mdecoder->decoder;
	if (!amr) {
		M_LOGE("amr null !\n");
		return -1;
	}

	Decoder_Interface_exit(amr->amrdec);

	snd_free(amr);

	M_LOGD("amr decoder release\n");
	return 0;
}

