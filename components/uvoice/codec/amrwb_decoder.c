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

#include "opensource/amrwb/dec_if.h"


#define AMRWB_MAGIC_NUMBER		"#!AMR-WB\n"

typedef struct {
	void *amrwbdec;
} amrwb_decoder_t;

extern const UWord8 block_size[];

static int amrwb_decoder_reset(void *priv)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	amrwb_decoder_t *amrwb;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	amrwb = mdecoder->decoder;
	if (!amrwb) {
		M_LOGE("amrwb decoder null !\n");
		return -1;
	}

	D_IF_reset(amrwb->amrwbdec);
	M_LOGD("amrwb decoder reset\n");
	return 0;
}

static int amrwb_decoder_process(void *priv, const uint8_t *buffer, int nbytes)
{
	media_decoder_t *mdecoder = (media_decoder_t *)priv;
	amrwb_decoder_t *amrwb;
	const uint8_t *readptr = buffer;
	uint8_t frame_header;
	int rem_size = nbytes;
	int frame_size;
	short *out_buffer;
	int ret;

	int16_t dec_mode;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	amrwb = mdecoder->decoder;
	if (!amrwb) {
		M_LOGE("amrwb decoder null !\n");
		return -1;
	}

	out_buffer = mdecoder->buffer_out;

	if (!mdecoder->running) {
		#ifndef IF2
		if (memcmp(buffer, AMRWB_MAGIC_NUMBER, strlen(AMRWB_MAGIC_NUMBER))) {
			M_LOGE("amrwb magic number not found !\n");
			return -1;
		}
		readptr += strlen(AMRWB_MAGIC_NUMBER);
		rem_size -= strlen(AMRWB_MAGIC_NUMBER);
		#endif

		media_pcminfo_t pcm_info;
		memset(&pcm_info, 0, sizeof(pcm_info));
		pcm_info.rate = 16000;
		pcm_info.frames = L_FRAME16k;
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
		#ifdef IF2
		dec_mode = (frame_header >> 4);
		#else
		dec_mode = (frame_header >> 3) & 0xf;
		#endif
		rem_size--;
		frame_size = block_size[dec_mode] - 1;
		if (rem_size < frame_size) {
			mdecoder->unproc_size = rem_size + 1;
			break;
		}

		if (frame_header & 0x04) {
			//M_LOGD("frame_size %d rem_size %d\n", frame_size, rem_size);
			D_IF_decode(amrwb->amrwbdec, readptr, out_buffer, _good_frame);

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

static int amrwb_decoder_action(void *priv, player_action_t action, void *arg)
{
	return 0;
}

int amrwb_decoder_create(media_decoder_t *mdecoder)
{
	amrwb_decoder_t *amrwb;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	amrwb = snd_zalloc(sizeof(amrwb_decoder_t), AFM_MAIN);
	if (!amrwb) {
		M_LOGE("alloc amrwb decoder failed !\n");
		return -1;
	}

	amrwb->amrwbdec = D_IF_init();
	if (!amrwb->amrwbdec) {
		M_LOGE("init amrwbdec failed !\n");
		snd_free(amrwb);
		return -1;
	}

	mdecoder->buffer_out_size = sizeof(short) * L_FRAME16k;
	mdecoder->buffer_out = snd_zalloc(mdecoder->buffer_out_size, AFM_MAIN);
	if (!mdecoder->buffer_out) {
		M_LOGE("alloc out buffer failed !\n");
		snd_free(amrwb);
		return -1;
	}

	mdecoder->input_size = 256;

	mdecoder->decode = amrwb_decoder_process;
	mdecoder->action = amrwb_decoder_action;
    mdecoder->reset = amrwb_decoder_reset;
    mdecoder->decoder = amrwb;

	M_LOGD("amrwb decoder create\n");
	return 0;
}

int amrwb_decoder_release(media_decoder_t *mdecoder)
{
	amrwb_decoder_t *amrwb;

	if (!mdecoder) {
		M_LOGE("mdecoder null !\n");
		return -1;
	}

	if (mdecoder->buffer_out) {
		snd_free(mdecoder->buffer_out);
		mdecoder->buffer_out = NULL;
		mdecoder->buffer_out_size = 0;
	}

    amrwb = mdecoder->decoder;
	if (!amrwb) {
		M_LOGE("amrwb null !\n");
		return -1;
	}

	D_IF_exit(amrwb->amrwbdec);

	snd_free(amrwb);

	M_LOGD("amrwb decoder release\n");
	return 0;
}

